#pragma once
#include "Object.h"

BEGIN_NAMESPACE_SPECTRE

class Object3D :public Object
{
public:
	SpectreObject(Object3D)

	virtual ~Object3D();
};

END_NAMESPACE_SPECTRE