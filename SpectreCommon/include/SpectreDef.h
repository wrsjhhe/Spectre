#pragma once
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#define SPECTRE_ENGINE_VERSION 10000

#define BEGIN_NAMESPACE(Name) \
        namespace Name                     \
        {

#define END_NAMESPACE(Name) }

#define USING_NAMESPACE(Name) using namespace Name;

#define BEGIN_NAMESPACE_SPECTRE BEGIN_NAMESPACE(Spectre)
#define END_NAMESPACE_SPECTRE END_NAMESPACE(Spectre)

#if !defined(NDEBUG) || defined(DEBUG) || defined(_DEBUG)
#	define SPE_DEBUG
#endif

#define _HAS_CXX17 1