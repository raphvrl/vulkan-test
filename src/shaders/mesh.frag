#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;

void main()
{
    vec3 color = fragColor;
    outColor = vec4(color, 1.0);
}