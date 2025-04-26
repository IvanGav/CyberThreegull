#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require
#extension GL_EXT_scalar_block_layout : require

struct M4x3F {
	vec4 row0;
	vec4 row1;
	vec4 row2;
};

layout (scalar, buffer_reference, buffer_reference_align = 16) readonly buffer ModelMatrices {
	M4x3F matrices[];
};

layout (scalar, buffer_reference, buffer_reference_align = 16) readonly buffer PositionData {
	vec3 data[];
};
layout (scalar, buffer_reference, buffer_reference_align = 16) readonly buffer TexcoordData {
	vec2 data[];
};
layout (scalar, buffer_reference, buffer_reference_align = 16) readonly buffer NormalData {
	vec3 data[];
};

layout (scalar, push_constant) uniform RenderData {
	uvec2 positionBufferPointer;
	uvec2 texcoordBufferPointer;
	uvec2 normalBufferPointer;
	M4x3F viewProj;
	M4x3F model;
	int texture;
	uint color;
} renderData;

layout (location = 0) out vec2 pos;
layout (location = 1) out vec2 texcoord;
layout (location = 2) out vec4 color;
layout (location = 3) out vec3 norm;
layout (location = 4) flat out uint texidx;

void main(){
	vec4 pos = vec4(PositionData(renderData.positionBufferPointer).data[gl_VertexIndex], 1.0);
	vec2 tex = TexcoordData(renderData.texcoordBufferPointer).data[gl_VertexIndex];
	M4x3F viewProjectionMat = renderData.viewProj;
	M4x3F modelMat = renderData.model;

	texcoord = tex;
	color = unpackUnorm4x8(renderData.color);
	texidx = renderData.texture;
	norm = NormalData(renderData.normalBufferPointer).data[gl_VertexIndex];

	vec4 transformedPos = vec4(
		dot(pos, modelMat.row0),
		dot(pos, modelMat.row1),
		dot(pos, modelMat.row2),
		1.0F
	);

	pos = transformedPos;

	float x = dot(transformedPos, viewProjectionMat.row0);
	float y = dot(transformedPos, viewProjectionMat.row1);
	float z = 0.05F; // Near plane
	// row2 is actually row3 in  this case, since the matrix is a ProjectiveTransformMatrix
	float w = dot(transformedPos, viewProjectionMat.row2);

	gl_Position = vec4(x, -y, z, w);
}