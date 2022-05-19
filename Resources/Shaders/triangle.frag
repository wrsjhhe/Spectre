#version 450

layout (location = 0) in vec3 inColor;
layout (location = 1)  in  vec2 fragTexCoord;

layout (std140,set = 2,binding = 0) uniform MaterialBuffer
{
	ivec4 MaterialParams0;
	vec3  Color;
} MaterialData;

layout(set = 2,binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	if(MaterialData.MaterialParams0[1] == 1)
	{
		outFragColor = texture(texSampler, fragTexCoord);
	}
	else
	{
		if(MaterialData.MaterialParams0[0] == 1)
		{
			outFragColor = vec4(normalize(inColor),1.0);
		}
		else if(MaterialData.MaterialParams0[0]  == 0)
		{
			outFragColor = vec4(MaterialData.Color,1.0);
		}
	}
}