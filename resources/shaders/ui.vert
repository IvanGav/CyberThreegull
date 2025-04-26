#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_buffer_reference_uvec2 : require
#extension GL_EXT_scalar_block_layout : require

struct M4x3F {
	vec4 row0;
	vec4 row1;
	vec4 row2;
};

struct Vertex {
	vec3 pos;
	vec2 tex;
	uint color;
	uint textureIndex;
	uint flags;
};

layout (scalar, buffer_reference, buffer_reference_align = 16) readonly buffer VertexData {
	Vertex data[];
};
layout (scalar, buffer_reference, buffer_reference_align = 16) readonly buffer ClipBoxData {
	vec4 data[];
};

layout (scalar, push_constant) uniform RenderData {
	vec2 screenDimensions;
	uvec2 vertexBufferPointer;
	uvec2 clipBoxesPointer;
	M4x3F model;
	M4x3F viewProj;
} renderData;

layout (location = 0) out vec2 pos;
layout (location = 1) out vec2 texcoord;
layout (location = 2) out vec4 color;
layout (location = 3) flat out uint texidx;
layout (location = 4) flat out uint flags;
layout (location = 5) flat out vec4 clipBox;

void main(){
	Vertex vert = VertexData(renderData.vertexBufferPointer).data[gl_VertexIndex];
	pos = vert.pos.xy;
	texcoord = vert.tex;
	color = unpackUnorm4x8(vert.color);
	texidx = vert.textureIndex;
	flags = vert.flags;

	vec4 vpos = vec4(-vert.pos, 1.0);
	vec4 transformedPos = vec4(
		dot(vpos, renderData.model.row0),
		dot(vpos, renderData.model.row1),
		dot(vpos, renderData.model.row2),
		1.0F
	);

	float x = dot(transformedPos, renderData.viewProj.row0);
	float y = dot(transformedPos, renderData.viewProj.row1);
	float z = 0.05F; // Near plane
	// row2 is actually row3 in  this case, since the matrix is a ProjectiveTransformMatrix
	float w = dot(transformedPos, renderData.viewProj.row2);

	if (renderData.clipBoxesPointer != uvec2(0, 0)) {
		clipBox = ClipBoxData(renderData.clipBoxesPointer).data[vert.flags >> 16];
	}
	gl_Position = vec4(x, -y, z, w);
}