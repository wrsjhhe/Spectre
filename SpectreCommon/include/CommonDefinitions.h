#pragma once

#define SPECTRE_BEGIN_NAMESPACE(Name) \
        namespace Name                     \
        {

#define SPECTRE_END_NAMESPACE(Name) }

#define SPECTRE_USING_NAMESPACE(Name) using namespace Name;