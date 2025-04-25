#version 460
#extension GL_EXT_nonuniform_qualifier : require

#define UI_RENDER_FLAG_MSDF 1

layout (set = 0, binding = 0) uniform sampler bilinearSampler;
layout (set = 0, binding = 1) uniform texture2D textures[];

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec4 color;
layout (location = 3) flat in uint texidx;

void main(){
	fragColor = texture(sampler2D(textures[nonuniformEXT(texidx)], bilinearSampler), texcoord) * color;
}