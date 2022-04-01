#pragma once
#include <string>
#include <vector>
#include <memory>
#include "BufferMaterial.h"
#include "Math/MathDef.h"
BEGIN_NAMESPACE_SPECTRE

class MeshBasicMaterial;
typedef std::shared_ptr<MeshBasicMaterial> MeshBasicMaterialPtr;
class MeshBasicMaterial :public BufferMaterial
{
public:
	struct ShaderVariable
	{
		alignas(16) int UserVextexColor;
		alignas(16) float Color[3];
		
	};

public:
	DefineClass(MeshBasicMaterial)

	static std::shared_ptr<MeshBasicMaterial> Create();

	void EnableVextexColor(bool enable);

	void SetColor(Color rgb);

	virtual MaterialBufferInfo GetBufferInfo() override;
public:
	
	virtual ~MeshBasicMaterial();

protected:
	MeshBasicMaterial();
	MeshBasicMaterial(const MeshBasicMaterial&) = delete;
};

typedef std::shared_ptr<MeshBasicMaterial> MeshBasicMaterialPtr;
END_NAMESPACE_SPECTRE