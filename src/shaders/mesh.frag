#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 fragUV;

layout(binding = 2) uniform sampler2D textures[];

void main()
{
    outColor = texture(textures[1], fragUV);
}