#pragma once
#include <string>
#include <vector>
#include <memory>
#include "BufferMaterial.h"
#include "Math/MathDef.h"
#include "VulkanTexture.h"
#include "VulkanDescriptor.h"
BEGIN_NAMESPACE_SPECTRE

class MeshBasicMaterial;
typedef std::shared_ptr<MeshBasicMaterial> MeshBasicMaterialPtr;
class MeshBasicMaterial :public BufferMaterial
{
public:
	struct ShaderVariable
	{
		int MaterialParams0[4];
		float Color[3];
	};

public:
	DefineClass(MeshBasicMaterial)

	virtual ~MeshBasicMaterial();

	static std::shared_ptr<MeshBasicMaterial> Create();

	void EnableVextexColor(bool enable);

	void SetColor(Color rgb);

	void SetTexture(const std::string& image);
	VulkanTexturePtr GetTexture() const { return m_Texture; }

	virtual MaterialBufferInfo GetBufferInfo() override;

	virtual VulkanDescriptorBuilder GetDescriptorBuilder() const override;

	virtual void CreateDescriptorSet() override;
protected:
	MeshBasicMaterial();
	MeshBasicMaterial(const MeshBasicMaterial&) = delete;

private:
	VulkanTexturePtr            m_Texture;
};

END_NAMESPACE_SPECTRE