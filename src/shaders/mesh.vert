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
    mat4 model;
    uint camUBOIndex;
} pc;

layout(location = 0) out vec2 fragUV;

void main()
{
    mat4 view = camUBO[0].view;
    mat4 proj = camUBO[0].proj;
    mat4 model = pc.model;

    gl_Position = proj * view * model * vec4(inPos, 1.0);
    fragUV = inUV;
}