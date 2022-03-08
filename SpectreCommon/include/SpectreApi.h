#pragma once
#include "SpectreDef.h"
BEGIN_NAMESPACE_SPECTRE


#define DefineClass(T)					 \
template<typename T>						 \
static bool Desc(T* ptr)					 \
{											 \
	return dynamic_cast<T*>(ptr) != nullptr; \
}											 \

typedef uint32_t SpectreId;

class SpectreApi
{
public:
	DefineClass(SpectreApi);

	SpectreApi(const SpectreApi&) = delete;
	SpectreApi(SpectreApi&&) = delete;
	SpectreApi& operator = (const SpectreApi&) = delete;
	SpectreApi& operator = (SpectreApi&&) = delete;


	virtual SpectreId Id() const
	{
		return (SpectreId)this;
	}

protected:
protected:
	SpectreApi() = default;
	~SpectreApi() = default;
};


END_NAMESPACE_SPECTRE