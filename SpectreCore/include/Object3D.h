#pragma once
#include "SpectreApi.h"

BEGIN_NAMESPACE_SPECTRE

class Object3D :public SpectreApi
{
public:
	DefineClass(Object3D)

	virtual ~Object3D();

	bool MatrixWorldNeedsUpdate() const { return m_MatrixWorldNeedsUpdate; }

protected:
	bool m_MatrixWorldNeedsUpdate = false;
};

END_NAMESPACE_SPECTRE