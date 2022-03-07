#pragma once
#include "SpectreApi.h"

BEGIN_NAMESPACE_SPECTRE

class Object3D :public SpectreApi
{
public:
	DefineClass(Object3D)

	virtual ~Object3D();
};

END_NAMESPACE_SPECTRE