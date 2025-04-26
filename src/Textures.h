#pragma once

#include "DrillLib.h"
#include "VK_decl.h"
#include "VKStaging_decl.h"
#include "PNG.h"
#include "Textures_decl.h"

namespace Textures {

struct NameTex {
	StrA name;
	StrA tex;
};

NameTex textureNames[]{
	{ "Sphere.001"a, "Brain Texture.png"a },
	{ "LOL.001"a, "images/LOL.png"a },
	{ "Microwave.001"a, "microwave.png"a },
	{ "Room.001"a, "room.png"a },
	{ "Terminal.001"a, "Terminal_screen.png"a },
	{ "Room.002"a, "images/gandr-collage.png"a },
	{ "Terminal.002"a, "Terminal_screen.png"a },
	{ "Fridge_2.001"a, "sewer.png"a },
	{ "exit.001"a, "sewer.png"a },
	{ "Sphere.002"a, "Brainrot Texture.png"a },
	{ "terminal"a, "Terminal_screen.png"a },
	{ "To_Do_List.001"a, "ToDo List.png"a },
	{ "2435"a, "images/brainrot/2435.png"a },
	{ "3432"a, "images/brainrot/3432.png"a },
	{ "6543"a, "images/brainrot/6543.png"a },
//	{ "12344"a, "images/brainrot/12344.png"a },
	{ "25435"a, "images/brainrot/25435.png"a },
	{ "43243"a, "images/brainrot/43243.png"a },
	{ "254543"a, "images/brainrot/254543.png"a },
	{ "266654"a, "images/brainrot/266654.png"a },
	{ "543254"a, "images/brainrot/543254.png"a },
	{ "5432543"a, "images/brainrot/5432543.png"a },
	{ "6543654"a, "images/brainrot/6543654.png"a },
	{ "54325432"a, "images/brainrot/54325432.png"a },
	{ "54326432"a, "images/brainrot/54326432.png"a },
	{ "bgfdb"a, "images/brainrot/bgfdb.png"a },
	{ "bgfdhgdf"a, "images/brainrot/bgfdhgdf.png"a },
	{ "brttd"a, "images/brainrot/brttd.png"a },
	{ "bvxcbvcx"a, "images/brainrot/bvxcbvcx.png"a },
	{ "feaef"a, "images/brainrot/feaef.png"a },
	{ "Cylinder.032"a, "Terminal_screen.png"a },
	{ "ffres"a, "images/brainrot/ffres.png"a },
	{ "fsfds"a, "images/brainrot/fsfds.png"a },
	{ "gggsgf"a, "images/brainrot/gggsgf.png"a },
	{ "ggtr"a, "images/brainrot/ggtr.png"a },
	{ "hhdfgd"a, "images/brainrot/hhdfgd.png"a },
	{ "hsfrs"a, "images/brainrot/hsfrs.png"a },
	{ "tgfs"a, "images/brainrot/tgfs.png"a },
	{ "tretwre"a, "images/brainrot/tretwre.png"a },
	{ "unknown"a, "images/brainrot/unknown.png"a },
	{ "vfsfvfsd"a, "images/brainrot/vfsfvfsd.png"a },
	{ "Badge"a, "badge.png"a },
	{ "Cell_Door_2"a, "Elevator.png"a },
	{ "Office"a, "Insecurity Room.png"a },
	{ "Terminal.004"a, "Terminal_screen.png"a },
	{ "Anxiety_Room"a, "anxiety room.png"a },
	{ "Lever"a, "lever.png"a },
	{ "Prompt_Management_Door"a, "Prompt Management Door.png"a },
	{ "Terminal.003"a, "Terminal_screen.png"a },
	{ "Trolley_Sign"a, "images/trolley problem.png"a },
	{ "Andrew_tate"a, "images/Untitled (15).png"a }
};

struct NameColor {
	StrA name;
	V3F32 color;
};

NameColor colorNames[]{
	{"Blanket.001"a,{20, 20, 20}},
	{"Curtain_1.001"a,{0, 0, 0}},
	{"Curtain_2.001"a,{0, 0, 0}},
	{"Fridge.001"a,{255, 255, 255}},
	{"light_cable.001"a,{117, 117, 117}},
	{"light_holder.001"a,{117, 117, 117}},
	{"lightbulb.001"a,{227, 226, 150}},
	{"Mattress.001"a,{0, 0, 0}},
	{"Pillow.001"a,{255, 255, 255}},
	{"Pillow.001"a,{255, 255, 255}},
	{"Couch"a,{138, 0, 0}},
	{"Entrance.001"a,{59, 59, 59}},
	{"fidget"a,{0, 39, 179}},
	{"fidget2"a,{0, 39, 179}},
	{"fidget3"a,{0, 39, 179}},
	{"fidget4"a,{0, 39, 179}},
	{"Red_rope_1"a,{179, 0, 0}},
	{"red_rope_2"a,{179, 0, 0}},
	{"red_rope_3"a,{179, 0, 0}},
	{"red_rope_4"a,{179, 0, 0}},
	{"Cell_Door.001"a,{43, 43, 43}},
	{"Cell_Door.002"a,{43, 43, 43}},
	{"Cell_Door.003"a,{43, 43, 43}},
	{"Cell_Door.004"a,{43, 43, 43}},
	{"Cell_Door.005"a,{43, 43, 43}},
	{"Cell_Door.006"a,{43, 43, 43}},
	{"Cell_Door.007"a,{43, 43, 43}},
	{"Desk"a,{54, 29, 8}},
	{"Security_Office_Text"a,{204, 141, 41}},
};


enum TextureFormat : U32 {
	TEXTURE_FORMAT_NULL,
	TEXTURE_FORMAT_RGBA_U8,
	TEXTURE_FORMAT_RGBA_U16,
	TEXTURE_FORMAT_RGBA_BC7,
	TEXTURE_FORMAT_COUNT
};
// I forgot what this enum was for. Perhaps to control mipmap generation? SRGB vs linear? I'm sure I needed it for something
enum TextureType : U32 {
	TEXTURE_TYPE_COLOR,
	TEXTURE_TYPE_NORMAL_MAP,
	TEXTURE_TYPE_MSDF
};
struct Texture {
	VkImage image;
	VkImageView imageView;
	U32 index;
	TextureType type;
};

Texture missing;
Texture simpleWhite;
Texture fontAtlas;
Texture uiIncrementLeft;
Texture uiIncrementRight;

Texture textures[ARRAY_COUNT(textureNames)];


ArenaArrayList<Texture*> allTextures;
ArenaArrayList<VkDeviceMemory> memoryUsed;
const U64 blockAllocationSize = 32 * MEGABYTE;
U64 currentMemoryBlockOffset = 0;

void alloc_texture_memory(VkDeviceMemory* memoryOut, VkDeviceSize* allocatedOffsetOut, VkMemoryRequirements memoryRequirements) {
	if (!(memoryRequirements.memoryTypeBits & (1 << VK::deviceMemoryTypeIndex))) {
		abort("Texture can't be in device memory? Shouldn't happen.");
	}
	if (memoryUsed.empty() || I64(blockAllocationSize - ALIGN_HIGH(currentMemoryBlockOffset, memoryRequirements.alignment)) < I64(memoryRequirements.size)) {
		VkMemoryAllocateInfo allocInfo{ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocInfo.allocationSize = max(blockAllocationSize, memoryRequirements.size);
		allocInfo.memoryTypeIndex = VK::deviceMemoryTypeIndex;
		VkDeviceMemory memory;
		CHK_VK(VK::vkAllocateMemory(VK::logicalDevice, &allocInfo, nullptr, &memory));
		memoryUsed.push_back(memory);
		currentMemoryBlockOffset = 0;
	}
	U64 allocatedOffset = ALIGN_HIGH(currentMemoryBlockOffset, memoryRequirements.alignment);
	currentMemoryBlockOffset = allocatedOffset + memoryRequirements.size;

	*memoryOut = memoryUsed.back();
	*allocatedOffsetOut = allocatedOffset;
}

void create_texture(Texture* result, void* data, U32 width, U32 height, TextureFormat format, TextureType type) {
	Texture& tex = *result;
	tex.type = type;
	VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = type == TEXTURE_TYPE_COLOR ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.extent = VkExtent3D{ width, height, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount = 0;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	CHK_VK(VK::vkCreateImage(VK::logicalDevice, &imageCreateInfo, nullptr, &tex.image));

	VkMemoryRequirements memoryRequirements;
	VK::vkGetImageMemoryRequirements(VK::logicalDevice, tex.image, &memoryRequirements);

	VkDeviceMemory linearBlock;
	VkDeviceSize memoryOffset;
	alloc_texture_memory(&linearBlock, &memoryOffset, memoryRequirements);
	CHK_VK(VK::vkBindImageMemory(VK::logicalDevice, tex.image, linearBlock, memoryOffset));

	VkCommandBuffer stagingCmdBuf = VK::graphicsStager.prepare_for_image_upload(width, height, sizeof(RGBA8));

	VkImageMemoryBarrier barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.srcAccessMask = VK_ACCESS_NONE_KHR;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.srcQueueFamilyIndex = VK::graphicsFamily;
	barrier.dstQueueFamilyIndex = VK::graphicsFamily;
	barrier.image = tex.image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	VK::vkCmdPipelineBarrier(stagingCmdBuf, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	if (data) {
		VK::graphicsStager.upload_to_image(tex.image, data, width, height, sizeof(RGBA8), 0);

		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_NONE_KHR;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		VK::vkCmdPipelineBarrier(stagingCmdBuf, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}

	VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	imageViewCreateInfo.image = tex.image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = imageCreateInfo.format;
	imageViewCreateInfo.components = VkComponentMapping{ VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	imageViewCreateInfo.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS };
	CHK_VK(VK::vkCreateImageView(VK::logicalDevice, &imageViewCreateInfo, nullptr, &tex.imageView));

	allTextures.push_back(result);

	if (currentTextureCount == currentTextureMaxCount) {
		currentTextureMaxCount *= 2;
		VK::drawDataDescriptorSet.change_dynamic_array_length(currentTextureMaxCount);
	}

	VkWriteDescriptorSet write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	write.dstSet = VK::drawDataDescriptorSet.descriptorSet;
	write.dstBinding = VK::drawDataDescriptorSet.bindingCount - 1;
	write.dstArrayElement = currentTextureCount;
	write.descriptorCount = 1;
	write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	VkDescriptorImageInfo imageInfo;
	imageInfo.sampler = VK_NULL_HANDLE;
	imageInfo.imageView = tex.imageView;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	write.pImageInfo = &imageInfo;
	VK::vkUpdateDescriptorSets(VK::logicalDevice, 1, &write, 0, nullptr);

	tex.index = currentTextureCount;
	currentTextureCount++;
}

void load_png(Texture* result, StrA path) {
	MemoryArena& stackArena = get_scratch_arena();
	MEMORY_ARENA_FRAME(stackArena) {
		RGBA8* image;
		U32 width, height;
		PNG::read_image(stackArena, &image, &width, &height, path);
		if (image) {
			create_texture(result, image, width, height, TEXTURE_FORMAT_RGBA_U8, TEXTURE_TYPE_COLOR);
		} else {
			*result = missing;
		}
	}
}

void load_msdf(Texture* result, StrA path) {
	MemoryArena& stackArena = get_scratch_arena();
	MEMORY_ARENA_FRAME(stackArena) {
		U32 fileSize = 0;
		Byte* file = read_full_file_to_arena<Byte>(&fileSize, stackArena, path);
		if (file) {
			U32 width = LOAD_LE32(file);
			U32 height = LOAD_LE32(file + sizeof(U32));
			create_texture(result, file + sizeof(U32) * 2, width, height, TEXTURE_FORMAT_RGBA_U8, TEXTURE_TYPE_MSDF);
		} else {
			*result = missing;
		}

	}
}

void load_all() {
	allTextures.reserve(256);
	memoryUsed.reserve(256);

	RGBA8 hardcodedTextureData[16 * 16];
	// The classic purple and black checker pattern
	for (U32 y = 0; y < 16; y++) {
		for (U32 x = 0; x < 16; x++) {
			hardcodedTextureData[y * 16 + x] = x < 8 == y < 8 ? RGBA8{ 255, 0, 255, 255 } : RGBA8{ 0, 0, 0, 255 };
		}
	}
	create_texture(&missing, hardcodedTextureData, 16, 16, TEXTURE_FORMAT_RGBA_U8, TEXTURE_TYPE_COLOR);
	memset(hardcodedTextureData, 0xFF, sizeof(hardcodedTextureData));
	create_texture(&simpleWhite, hardcodedTextureData, 16, 16, TEXTURE_FORMAT_RGBA_U8, TEXTURE_TYPE_COLOR);
	load_png(&uiIncrementLeft, "resources/textures/ui_increment_left.png"a);
	load_png(&uiIncrementRight, "resources/textures/ui_increment_right.png"a);
	load_msdf(&fontAtlas, "resources/textures/font.ddf"a);

	for (U32 i = 0; i < ARRAY_COUNT(textures); i++) {
		load_png(&textures[i], stracat(globalArena, "resources/textures/"a, textureNames[i].tex));
	}


}

void destroy_all() {
	for (Texture* tex : allTextures) {
		VK::vkDestroyImageView(VK::logicalDevice, tex->imageView, nullptr);
		VK::vkDestroyImage(VK::logicalDevice, tex->image, nullptr);
	}
	allTextures.clear();
	for (VkDeviceMemory mem : memoryUsed) {
		VK::vkFreeMemory(VK::logicalDevice, mem, nullptr);
	}
	memoryUsed.clear();
}

}