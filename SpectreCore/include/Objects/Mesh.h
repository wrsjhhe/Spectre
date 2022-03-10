#pragma once
#include "Object3D.h"
#include "Geometries/BufferGeometry.h"
#include "Materials/BufferMaterial.h"
#include "Math/MathDef.h"
#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"

BEGIN_NAMESPACE_SPECTRE

class Mesh;
typedef std::shared_ptr<Mesh> MeshPtr;
class Mesh : public Object3D
{
public:
	DefineClass(Mesh)
	static Mesh* Create(BufferGeometry* pGeometry, BufferMaterialPtr pMaterial);
public:

	virtual ~Mesh();

	void SetBindVertexBuffer(VertexBufferPtr pBuffer) { m_BindVertexBuffer = pBuffer; }
	void SetBindIndexBuffer(IndexBufferPtr pBuffer) { m_BindIndexBuffer = pBuffer; }
	void SetVertexBufferOffset(uint32_t offset) { m_VertexBufferOffset = offset; }
	void SetIndexBufferOffset(uint32_t offset) { m_IndexBufferOffset = offset; }


	BufferGeometryPtr GetBufferGeometry() const { return m_GeometryPtr; }
	BufferMaterialPtr GetMaterial() const { return m_MaterialPtr; }
	VertexBufferPtr GetBindVertexBuffer() const{ return m_BindVertexBuffer; }
	IndexBufferPtr GetBindIndexBuffer() const { return m_BindIndexBuffer; }
	uint32_t GetVertexBufferOffset() const { return m_VertexBufferOffset; }
	uint32_t GetIndexBufferOffset() const { return m_IndexBufferOffset; }

	void Transform(const Matrix& mat);

	const Matrix& GetTransformMatrix()const { return m_Transform; }
private:
	Mesh(BufferGeometry* pGeometry, BufferMaterialPtr pMaterial);

private:
	BufferGeometryPtr				m_GeometryPtr;
	BufferMaterialPtr				m_MaterialPtr;
	Matrix                          m_Transform;
	VertexBufferPtr                 m_BindVertexBuffer;
	IndexBufferPtr					m_BindIndexBuffer;
	uint32_t                        m_VertexBufferOffset = 0;
	uint32_t                        m_IndexBufferOffset = 0;
};

END_NAMESPACE_SPECTRE