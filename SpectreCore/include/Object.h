#pragma once
#include "SpectreDef.h"
BEGIN_NAMESPACE_SPECTRE


#define SpectreObject(T)					 \
template<typename T>						 \
static bool Desc(T* ptr)					 \
{											 \
	return dynamic_cast<T*>(ptr) != nullptr; \
}											 \

class Object
{
public:
	SpectreObject(Object);

	virtual uint32_t Id();

	virtual ~Object();
};

END_NAMESPACE_SPECTRE