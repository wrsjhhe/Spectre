#include "SpectreDef.h"
#include "Materials/MeshBasicMaterial.h"
#include "ShaderTool.h"
#include "FileUtils.h"
#include "Math/Vertex.h"
#include "Alignment.h"
USING_NAMESPACE(Spectre)


std::shared_ptr<MeshBasicMaterial> MeshBasicMaterial::Create()
{
	auto* pMat = new MeshBasicMaterial;
	return std::shared_ptr<MeshBasicMaterial>(pMat);
}

void MeshBasicMaterial::EnableVextexColor(bool enable)
{
	ShaderVariable* sv = reinterpret_cast<ShaderVariable*>(m_MaterialBuffer);
	sv->MaterialParams0[0] = enable?1:0;
}

void MeshBasicMaterial::SetColor(Color rgb)
{
	auto v3 = rgb.ToVector3();
	v3 /= 255.f;
	ShaderVariable* sv = reinterpret_cast<ShaderVariable*>(m_MaterialBuffer);
	std::memcpy(sv->Color, &v3, sizeof(sv->Color));
}

void MeshBasicMaterial::SetTexture(const std::string& image)
{
	int texWidth, texHeight;
	auto* pixels = FileUtils::ReadImageFile(image, &texWidth, &texHeight);
	m_Texture = VulkanTexture::Create2D(pixels, texWidth, texHeight);

	ShaderVariable* sv = reinterpret_cast<ShaderVariable*>(m_MaterialBuffer);
	sv->MaterialParams0[1] = 1;
}

MaterialBufferInfo MeshBasicMaterial::GetBufferInfo()
{
	constexpr auto a = sizeof(ShaderVariable);
	return {
		sizeof(ShaderVariable),
		m_MaterialBuffer
	};
}


VulkanDescriptorBuilder MeshBasicMaterial::GetDescriptorBuilder() const
{
	ShaderVariable* sv = reinterpret_cast<ShaderVariable*>(m_MaterialBuffer);
	VulkanDescriptorBuilder builder;
	builder.AddBind(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);  //ÑÕÉ«
	builder.AddBind(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);  //ÌùÍ¼
	
	return builder;
}

void MeshBasicMaterial::CreateDescriptorSet()
{
	uint64_t alignSize = VulkanEngine::GetInstance()->GetVkPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
	uint32_t materialBufferSize = Spectre::Align(sizeof(ShaderVariable), alignSize);

	m_VulkanBuffer = VulkanBuffer::Create(materialBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	m_VulkanBuffer->Map(m_MaterialBuffer);

	VkDescriptorBufferInfo descInfo;
	descInfo.buffer = m_VulkanBuffer->GetVkBuffer();
	descInfo.offset = 0;
	descInfo.range = materialBufferSize;

	ShaderVariable* sv = reinterpret_cast<ShaderVariable*>(m_MaterialBuffer);
	VkDescriptorImageInfo imageInfo;
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = m_Texture->GetImageView()->GetVkImageView();
	imageInfo.sampler = m_Texture->GetVkSampler();
	m_DescriptorSet = GetDescriptorBuilder().Build({ &descInfo }, { &imageInfo });
	//if (sv->MaterialParams0[1] == 1)
	//{
	//	VkDescriptorImageInfo imageInfo;
	//	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//	imageInfo.imageView = m_Texture->GetImageView()->GetVkImageView();
	//	imageInfo.sampler = m_Texture->GetVkSampler();

	//	m_DescriptorSet = GetDescriptorBuilder().Build({ &descInfo }, { &imageInfo });
	//}
	//else
	//{
	//	m_DescriptorSet = GetDescriptorBuilder().Build({ &descInfo });
	//}
}

MeshBasicMaterial::MeshBasicMaterial()
{
	SetVertexShader(FileUtils::ReadFile("Shaders/triangle.vert"));
	SetFragmentShader(FileUtils::ReadFile("Shaders/triangle.frag"));

	std::vector<VertexAttribute> VertexAttrs = Vertex::VertexAttributes;
	DefineAttributes(VertexAttrs);
	m_MaterialBuffer = new ShaderVariable{
		{0},
		{255.f,255.f,255.f}
	};

}


MeshBasicMaterial::~MeshBasicMaterial()
{

}


