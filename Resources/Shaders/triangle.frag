#version 450

layout (location = 0) in vec3 inColor;

layout (std140,set = 1,binding = 1) uniform MaterialBuffer
{
	int UserVextexColor;
	vec3 Color;
} MaterialData;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	if(MaterialData.UserVextexColor == 1)
	{
		outFragColor = vec4(normalize(inColor),1.0);
	}
	if(MaterialData.UserVextexColor == 0)
	{
		outFragColor = vec4(MaterialData.Color,1.0);
	}
}