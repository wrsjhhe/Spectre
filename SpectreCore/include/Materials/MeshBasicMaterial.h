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
		/// <summary>
		/// index0,displaymode:0BaseColorMode,1VetexColorMode,2TextureMode
		/// </summary>
		int MaterialParams0[4] = {0};
		float Color[3] = {0.f,0.f,0.f };
	};

public:
	DefineClass(MeshBasicMaterial)

	virtual ~MeshBasicMaterial();

	static std::shared_ptr<MeshBasicMaterial> Create();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="mode">0,BaseColorMode;1,VetexColorMode;2,TextureMode</param>
	void SetDisplayMode(int mode);

	void SetColor(Color rgb);

	void SetTexture(const std::string& image);
	VulkanTexturePtr GetTexture() const { return m_Texture; }

	virtual MaterialBufferInfo GetBufferInfo() override;

	virtual VulkanDescriptorBuilder GetDescriptorBuilder() const override;

	virtual void CreateDescriptorSet() override;

	virtual void Update(int k = 0) override;
protected:
	MeshBasicMaterial();
	MeshBasicMaterial(const MeshBasicMaterial&) = delete;

private:
	VulkanTexturePtr            m_Texture = VulkanTexture::CreaetInvalid();
};

END_NAMESPACE_SPECTRE