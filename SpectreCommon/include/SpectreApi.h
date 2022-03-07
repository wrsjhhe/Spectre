#pragma once
#include "SpectreDef.h"
BEGIN_NAMESPACE_SPECTRE


#define DefineClass(T)					 \
template<typename T>						 \
static bool Desc(T* ptr)					 \
{											 \
	return dynamic_cast<T*>(ptr) != nullptr; \
}											 \

typedef uint32_t SpectrId;

class SpectreApi
{
public:
	DefineClass(SpectreApi);

	SpectreApi(const SpectreApi&) = delete;
	SpectreApi(SpectreApi&&) = delete;
	SpectreApi& operator = (const SpectreApi&) = delete;
	SpectreApi& operator = (SpectreApi&&) = delete;


	virtual SpectrId Id()
	{
		return (SpectrId)this;
	}

protected:
protected:
	SpectreApi() = default;
	~SpectreApi() = default;
};


END_NAMESPACE_SPECTRE