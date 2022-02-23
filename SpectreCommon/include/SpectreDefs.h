#pragma once
#include <stdint.h>
#define SPECTRE_ENGINE_VERSION 10000

#define BEGIN_NAMESPACE(Name) \
        namespace Name                     \
        {

#define END_NAMESPACE(Name) }

#define USING_NAMESPACE(Name) using namespace Name;

#define BEGIN_NAMESPACE_SPECTRE BEGIN_NAMESPACE(Spectre)
#define END_NAMESPACE_SPECTRE END_NAMESPACE(Spectre)