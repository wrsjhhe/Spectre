#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;

layout (set = 0,binding = 0) uniform CameraBuffer
{
	mat4 View;
	mat4 Projection;
}CameraData

layout (binding = 0) uniform UBO 
{
	mat4 MVPMatrix;
} ubo;

layout (location = 0) out vec3 outColor;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main() 
{
	outColor = inColor;
	gl_Position = ubo.MVPMatrix* vec4(inPos.xyz, 1.0);
}
