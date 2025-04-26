#version 460
#extension GL_EXT_nonuniform_qualifier : require

layout (set = 0, binding = 0) uniform sampler bilinearSampler;
layout (set = 0, binding = 1) uniform texture2D textures[];

layout (location = 0) out vec4 fragColor;

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec4 color;
layout (location = 3) in vec3 norm;
layout (location = 4) flat in uint texidx;

void main(){
	vec3 lightDir = vec3(0.57735026919F, 0.57735026919F, 0.57735026919F);
	vec4 resultColor = texture(sampler2D(textures[nonuniformEXT(texidx)], bilinearSampler), vec2(texcoord.x, 1.0 - texcoord.y)) * color;
	fragColor = vec4(resultColor.rgb * (dot(normalize(norm), lightDir) * 0.5F + 0.5F), resultColor.a);
}