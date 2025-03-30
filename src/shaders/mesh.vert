#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform CamUniforms {
    mat4 view;
    mat4 proj;
} camUBO[];

layout(push_constant) uniform PushConstants {
    uint camUBOIndex;
} pc;

void main()
{
    mat4 view = camUBO[pc.camUBOIndex].view;
    mat4 proj = camUBO[pc.camUBOIndex].proj;

    gl_Position = proj * view * vec4(inPos, 1.0);
}