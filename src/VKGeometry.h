#pragma once
#include "DrillLib.h"
#include "VK_decl.h"

namespace VKGeometry {

struct Bone {
	static constexpr U32 PARENT_INVALID_IDX = U32_MAX;

	M4x3F32 bindTransform;
	M4x3F32 invBindTransform;
	U32 parentIdx;
};

struct Skeleton {
	// Skeletons have a linked list of freed transforms. If the list is empty, allocate a new one
	U32 nextFreeTransformPointer;
	U32 boneCount;
	// Technically only valid in C, not C++, but MSVC supports it, and other compilers have extensions for flexible array members if I ever decide to switch
	Bone bones[];
};

struct StaticMesh {
	U32 indicesOffset;
	U32 verticesOffset;
	U32 indicesCount;
	U32 verticesCount;
};

struct SceneElement {
	StrA name;
	StaticMesh mesh;
};
struct StaticScene {
	SceneElement* elements;
	U32 elementCount;

	void draw_all(VkCommandBuffer cmdBuf) {
		for (U32 i = 0; i < elementCount; i++) {
			VK::vkCmdDrawIndexed(cmdBuf, elements[i].mesh.indicesCount, 1, elements[i].mesh.indicesOffset, I32(elements[i].mesh.verticesOffset), 0);
		}
	}
	void draw_named(VkCommandBuffer cmdBuf, StrA name) {
		for (U32 i = 0; i < elementCount; i++) {
			if (elements[i].name == name) {
				VK::vkCmdDrawIndexed(cmdBuf, elements[i].mesh.indicesCount, 1, elements[i].mesh.indicesOffset, I32(elements[i].mesh.verticesOffset), 0);
				break;
			}
		}
	}
};

struct SkeletalMesh {
	StaticMesh geometry;
	// Several meshes may have the same skeleton structure (e.g. multiple humanoid models animated the same way)
	Skeleton* skeletonData;
	U32 skinningDataOffset;
};

struct SkeletalAnimation {
	U32 keyframeCount;
	U32 boneCount;
	F32 framerate;
	U32 lengthMilliseconds;
	// Array of pose arrays
	// Array contains one pose array for each keyframe, and each pose array contains one matrix for each bone
	M4x3F32* matrices;
};

struct StaticModel {
	StaticMesh* mesh;
	M4x3F32 transform;
	U32 gpuMatrixIndex;
	V4F32 color;
};

struct SkeletalModel {
	SkeletalMesh* mesh;
	M4x3F32* poseMatrices;
	M4x3F32 transform;
	U32 gpuMatrixIndex;
	U32 skinnedVerticesOffset;
	U32 skeletonMatrixOffset;
};

#pragma pack(push, 1)
struct GPUSkinnedModel {
	U32 matricesOffset;
	U32 vertexOffset;
	U32 skinnedVerticesOffset;
	U32 skinningDataOffset;
	U32 vertexCount;
};
#pragma pack(pop)

struct GeometryHandler {
	VkDeviceMemory memory;
	VkDeviceSize memorySize;
	VkBuffer buffer;
	VkDeviceAddress gpuAddress;
	U64 indicesOffset;
	U64 positionsOffset;
	U64 texcoordsOffset;
	U64 normalsOffset;
	U64 tangentsOffset;
	U64 skinDataOffset;
	U64 skinnedPositionsOffset;
	U64 skinnedNormalsOffset;
	U64 skinnedTangentsOffset;
	U32 indicesAllocationCap;
	U32 geometryVertexAllocationCap;
	U32 skinningVertexAllocationCap;
	U32 skinnedVertexAllocationCap;
	U32 indicesAllocationOffset;
	U32 geometryVertexAllocationOffset;
	U32 skinningVertexAllocationOffset;
	U32 skinnedVertexAllocationOffset;

	void init(VkDeviceSize size) {
		if (size < 4096) {
			abort("Geometry needs 4k bytes");
		}
		const U64 alignment = VK::physicalDeviceProperties.limits.minStorageBufferOffsetAlignment;
		const U64 indexSize = sizeof(U16);
		F64 vertexDataSizesTotal = F64(VK::VERTEX_FORMAT_POS3F_TEX2F_NORM3F_TAN3F_SIZE) + F64(VK::VERTEX_FORMAT_INDEX4u8_WEIGHT4unorm8_SIZE) + F64(VK::VERTEX_FORMAT_POS3F_NORM3F_TAN3F_SIZE) + F64(indexSize);
		// Approximate weights for how relatively many vertices we need space for in each section.
		// I'll come back to this later when I have actual data and set them accordingly
		// I'm not actually sure if I want to keep with static allocation like this.
		// Dynamic allocation could work better, since that could conform memory allocation to gameplay needs.
		// Anyway, I'm not going to think too hard about it now
		F64 indicesDataScaledSize = 0.15 * (F64(indexSize) / vertexDataSizesTotal);
		F64 geometryDataScaledSize = 0.35 * (F64(VK::VERTEX_FORMAT_POS3F_TEX2F_NORM3F_TAN3F_SIZE) / vertexDataSizesTotal);
		F64 skinningDataScaledSize = 0.2 * (F64(VK::VERTEX_FORMAT_INDEX4u8_WEIGHT4unorm8_SIZE) / vertexDataSizesTotal);
		F64 skinningGeometryScaledSize = 0.3 * (F64(VK::VERTEX_FORMAT_POS3F_NORM3F_TAN3F_SIZE) / vertexDataSizesTotal);

		U64 maxAllocatableIndices = min<U64>(U32_MAX, (U64(indicesDataScaledSize * F64(size)) - alignment) / indexSize);
		indicesAllocationCap = U32(maxAllocatableIndices);
		indicesOffset = 0;
		// Vertices must be at most I32_MAX, since 
		U64 maxAllocatableVertices = min<U64>(I32_MAX, (U64(geometryDataScaledSize * F64(size)) - alignment * 4) / VK::VERTEX_FORMAT_POS3F_TEX2F_NORM3F_TAN3F_SIZE);
		geometryVertexAllocationCap = U32(maxAllocatableVertices);
		positionsOffset = ALIGN_HIGH(indicesOffset + indicesAllocationCap * indexSize, alignment);
		texcoordsOffset = ALIGN_HIGH(positionsOffset + geometryVertexAllocationCap * sizeof(V3F32), alignment);
		normalsOffset = ALIGN_HIGH(texcoordsOffset + geometryVertexAllocationCap * sizeof(V2F32), alignment);
		tangentsOffset = ALIGN_HIGH(normalsOffset + geometryVertexAllocationCap * sizeof(V3F32), alignment);
		U64 maxAllocatableSkinningIndicesAndWeights = min<U64>(I32_MAX, (U64(skinningDataScaledSize * F64(size)) - alignment) / VK::VERTEX_FORMAT_INDEX4u8_WEIGHT4unorm8_SIZE);
		skinningVertexAllocationCap = U32(maxAllocatableSkinningIndicesAndWeights);
		skinDataOffset = ALIGN_HIGH(tangentsOffset + geometryVertexAllocationCap * sizeof(V3F32), alignment);
		U64 maxAllocatableSkinnedVertices = min<U64>(I32_MAX, (U64(skinningGeometryScaledSize * F64(size)) - alignment * 3) / VK::VERTEX_FORMAT_POS3F_NORM3F_TAN3F_SIZE);
		skinnedVertexAllocationCap = U32(maxAllocatableSkinnedVertices);
		skinnedPositionsOffset = ALIGN_HIGH(skinDataOffset + skinningVertexAllocationCap * U64(VK::VERTEX_FORMAT_INDEX4u8_WEIGHT4unorm8_SIZE), alignment);
		skinnedNormalsOffset = ALIGN_HIGH(skinnedPositionsOffset + skinnedVertexAllocationCap * sizeof(V3F32), alignment);
		skinnedTangentsOffset = ALIGN_HIGH(skinnedNormalsOffset + skinnedVertexAllocationCap * sizeof(V3F32), alignment);
		U64 finalSizeRequired = skinnedTangentsOffset + skinnedVertexAllocationCap * sizeof(V3F32);
		memorySize = finalSizeRequired;

		indicesAllocationOffset = 0;
		geometryVertexAllocationOffset = 0;
		skinningVertexAllocationOffset = 0;
		skinnedVertexAllocationOffset = 0;

		VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.flags = 0;
		bufferInfo.size = finalSizeRequired;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.queueFamilyIndexCount = 0;
		bufferInfo.pQueueFamilyIndices = nullptr;
		CHK_VK(VK::vkCreateBuffer(VK::logicalDevice, &bufferInfo, VK_NULL_HANDLE, &buffer));
		VkMemoryRequirements memoryRequirements;
		VK::vkGetBufferMemoryRequirements(VK::logicalDevice, buffer, &memoryRequirements);
		if (!(memoryRequirements.memoryTypeBits & (1 << VK::deviceMemoryTypeIndex))) {
			abort("Could not create geometry buffer in device local memory");
		}
		VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = VK::deviceMemoryTypeIndex;
		CHK_VK(VK::vkAllocateMemory(VK::logicalDevice, &allocInfo, nullptr, &memory));
		CHK_VK(VK::vkBindBufferMemory(VK::logicalDevice, buffer, memory, 0));

		VkBufferDeviceAddressInfo addressInfo{ VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO };
		addressInfo.buffer = buffer;
		gpuAddress = VK::vkGetBufferDeviceAddress(VK::logicalDevice, &addressInfo);
	}

	void alloc_static(U32* indicesOffsetOut, U32* verticesOffsetOut, U32 numIndices, U32 numVertices) {
		if (numVertices > U16_MAX) {
			abort("More vertices than maximum index, split the model or implement 32 bit indices");
		}
		U32 indicesStart = indicesAllocationOffset;
		indicesAllocationOffset = indicesStart + numIndices;
		if (indicesAllocationOffset > indicesAllocationCap) {
			abort("Ran out of memory for indices buffer. Perhaps time to implement streaming?");
		}
		U32 verticesStart = geometryVertexAllocationOffset;
		geometryVertexAllocationOffset = verticesStart + numVertices;
		if (geometryVertexAllocationOffset > geometryVertexAllocationCap) {
			abort("Ran out of memory for geometry vertex buffer. Perhaps time to implement streaming?");
		}
		*indicesOffsetOut = indicesStart;
		*verticesOffsetOut = verticesStart;
	}

	void alloc_skeletal(U32* indicesOffsetOut, U32* verticesOffsetOut, U32* skinningDataOffset, U32 numIndices, U32 numVertices) {
		if (numVertices > U16_MAX) {
			abort("More vertices than maximum index, split the model or implement 32 bit indices");
		}
		U32 indicesStart = indicesAllocationOffset;
		indicesAllocationOffset = indicesStart + numIndices;
		if (indicesAllocationOffset > indicesAllocationCap) {
			abort("Ran out of memory for indices buffer. Perhaps time to implement streaming?");
		}
		U32 verticesStart = geometryVertexAllocationOffset;
		geometryVertexAllocationOffset = verticesStart + numVertices;
		if (geometryVertexAllocationOffset > geometryVertexAllocationCap) {
			abort("Ran out of memory for geometry vertex buffer. Perhaps time to implement streaming?");
		}
		U32 skinningStart = skinningVertexAllocationOffset;
		skinningVertexAllocationOffset = skinningStart + numVertices;
		if (skinningVertexAllocationOffset > skinningVertexAllocationCap) {
			abort("Ran out of memory for skinning data buffer. Perhaps time to implement streaming?");
		}
		*indicesOffsetOut = indicesStart;
		*verticesOffsetOut = verticesStart;
		*skinningDataOffset = skinningStart;
	}

	// Skinned results should be allocated each frame.
	// This means CPU culled objects don't use skinned memory, and a cheap bump allocator can be used.
	U32 alloc_skinned_result(U32 numVertices) {
		U32 skinnedVerticesStart = skinnedVertexAllocationOffset;
		skinnedVertexAllocationOffset += numVertices;
		if (skinnedVertexAllocationOffset > skinnedVertexAllocationCap) {
			abort("Ran out of memory for skinned vertices result. Think of a way to handle this without crashing.");
		}
		return skinnedVerticesStart;
	}

	void reset_skinned_results() {
		skinnedVertexAllocationOffset = 0;
	}

	void destroy() {
		VK::vkDestroyBuffer(VK::logicalDevice, buffer, VK_NULL_HANDLE);
		VK::vkFreeMemory(VK::logicalDevice, memory, nullptr);
	}
};

struct UniformMatricesHandler {
	VkDeviceMemory memory;
	VkBuffer buffer;
	M4x3F32* memoryMapping;
	U32 maxMatrices;
	// Matrix is always at least 3. The first matrix is the identity matrix and the next two are eye matrices
	U32 matrixOffset;

	void init(U32 size) {
		if (size < 4096) {
			abort("Needs 4k of matrix data");
		}
		maxMatrices = size / sizeof(M4x3F32);
		matrixOffset = 3;

		VkBufferCreateInfo bufferInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.flags = 0;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferInfo.queueFamilyIndexCount = 0;
		bufferInfo.pQueueFamilyIndices = nullptr;
		CHK_VK(VK::vkCreateBuffer(VK::logicalDevice, &bufferInfo, nullptr, &buffer));
		VkMemoryRequirements memoryRequirements;
		VK::vkGetBufferMemoryRequirements(VK::logicalDevice, buffer, &memoryRequirements);
		if (!(memoryRequirements.memoryTypeBits & VK::hostMemoryTypeIndex)) {
			abort("Skinning matrix buffer did not support allocation in host memory");
		}
		VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocInfo.allocationSize = memoryRequirements.size;
		allocInfo.memoryTypeIndex = VK::hostMemoryTypeIndex;
		CHK_VK(VK::vkAllocateMemory(VK::logicalDevice, &allocInfo, nullptr, &memory));
		CHK_VK(VK::vkBindBufferMemory(VK::logicalDevice, buffer, memory, 0));
		CHK_VK(VK::vkMapMemory(VK::logicalDevice, memory, 0, size, 0, reinterpret_cast<void**>(&memoryMapping)));

		memoryMapping[0] = M4x3F32{}.set_identity();
	}

	void destroy() {
		VK::vkDestroyBuffer(VK::logicalDevice, buffer, nullptr);
		VK::vkFreeMemory(VK::logicalDevice, memory, nullptr);
	}

	U32 alloc_and_set(U32 numMatrices, M4x3F32* matrices) {
		U32 offset = matrixOffset;
		matrixOffset += numMatrices;
		if (matrixOffset > maxMatrices) {
			print("Ran out of space for transform matrices. Figure out a way to deal with this.");
			offset = 0;
		} else {
			for (U32 i = 0; i < numMatrices; i++) {
				memoryMapping[offset + i] = matrices[i];
			}
		}
		return offset;
	}

	U32 alloc(U32 numMatrices) {
		U32 offset = matrixOffset;
		matrixOffset += numMatrices;
		if (matrixOffset > maxMatrices) {
			print("Ran out of space for transform matrices. Figure out a way to deal with this.");
			offset = 0;
		}
		return offset;
	}

	void set_eye_matrices(ProjectiveTransformMatrix eye) {
		// Not really Matrix4x3f, since the third row is omitted instead of the fourth, but fits in a Matrix4x3f
		memoryMapping[1] = M4x3F32{
			eye.m00, eye.m01, eye.m02, eye.m03,
			eye.m10, eye.m11, eye.m12, eye.m13,
			eye.m30, eye.m31, eye.m32, eye.m33
		};
	}

	void flush_memory() {
		VkMappedMemoryRange memoryRange{ VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
		memoryRange.memory = memory;
		memoryRange.offset = 0;
		memoryRange.size = ALIGN_HIGH(matrixOffset * sizeof(M4x3F32), VK::physicalDeviceProperties.limits.nonCoherentAtomSize);
		CHK_VK(VK::vkFlushMappedMemoryRanges(VK::logicalDevice, 1, &memoryRange));
	}

	void reset() {
		// First 3 are identity and two eye matrices
		matrixOffset = 2;
	}
};

void make_static_model(StaticModel* model, StaticMesh& mesh) {
	model->mesh = &mesh;
	model->transform.set_identity();
	model->color = V4F32{ 1.0F, 1.0F, 1.0F, 1.0F };
}

void make_skeletal_model(SkeletalModel* model, SkeletalMesh& mesh) {
	model->mesh = &mesh;
	model->transform.set_identity();
}

}