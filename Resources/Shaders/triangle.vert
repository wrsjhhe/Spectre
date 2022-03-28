#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

layout (binding = 0) uniform CameraBuffer
{
	mat4 View;
	mat4 Projection;
}CameraData;

layout (binding = 1) uniform ModelBuffer 
{
	mat4 Transform;
} ModelData;

layout (location = 0) out vec3 outColor;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main() 
{
	outColor = inColor;
	gl_Position = CameraData.Projection * CameraData.View * ModelData.Transform * vec4(inPos.xyz, 1.0);
}
