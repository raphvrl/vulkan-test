#version 450

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragColor;

// layout(push_constant) uniform PushConstants {
//     mat4 proj;
//     mat4 view;
// } pco;

void main()
{
    gl_Position = vec4(inPos, 1.0);
    fragColor = vec3(inUV, 1.0);
}