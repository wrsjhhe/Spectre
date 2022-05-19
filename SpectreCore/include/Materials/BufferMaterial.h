#pragma once

#include "SpectreApi.h"
#include "VulkanEngine.h"
#include "VulkanDescriptor.h"
#include "VulkanBuffer.h"

BEGIN_NAMESPACE_SPECTRE

struct MaterialBufferInfo
{
	uint32_t Size;
	void* Buffer;
};

class BufferMaterial;
typedef std::shared_ptr<BufferMaterial> BufferMaterialPtr;
class BufferMaterial :public SpectreApi
{
public:
	DefineClass(BufferMaterial)
public:

	void SetVertexShader(std::string vert);
	void SetFragmentShader(std::string frag);

	const std::string& GetVertexShader()const { return m_VertexShader; }
	const std::string& GetFragmentShader()const { return m_FragmentShader; }

	void DefineAttributes(const std::vector<VertexAttribute>& attrs);
	const std::vector<VertexAttribute> GetAttributes() const { return m_Attributes; }

	virtual MaterialBufferInfo GetBufferInfo() = 0;

	virtual size_t GetHash();

	virtual VulkanDescriptorBuilder GetDescriptorBuilder() const = 0;

	virtual void CreateDescriptorSet() = 0;

	virtual VulkanDescriptorSetPtr GetDescriptorSet() const { return m_DescriptorSet; }

	virtual ~BufferMaterial();


protected:
	BufferMaterial();

	void* m_MaterialBuffer;
	VulkanDescriptorSetPtr          m_DescriptorSet;
	VulkanBufferPtr                 m_VulkanBuffer;
private:
	std::vector<VertexAttribute>	m_Attributes;
	std::string						m_VertexShader;
	std::string						m_FragmentShader;

};


END_NAMESPACE_SPECTRE