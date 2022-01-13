#pragma once

#define SPECTRE_ENGINE_VERSION 10000

#define BEGIN_NAMESPACE(Name) \
        namespace Name                     \
        {

#define END_NAMESPACE(Name) }

#define USING_NAMESPACE(Name) using namespace Name;

#define BEGIN_NAMESPACE_SPECTRE BEGIN_NAMESPACE(Spectre)
#define END_NAMESPACE_SPECTRE END_NAMESPACE(Spectre)

#define BEGIN_NAMESPACE_COMMON BEGIN_NAMESPACE(Spectre) \
                               BEGIN_NAMESPACE(Common)
#define END_NAMESPACE_COMMON END_NAMESPACE(Spectre) \
                             END_NAMESPACE(Common)

#define BEGIN_NAMESPACE_RENDERSYSTEMS BEGIN_NAMESPACE(Spectre) \
                               BEGIN_NAMESPACE(RenderSystems)
#define END_NAMESPACE_RENDERSYSTEMS END_NAMESPACE(Spectre) \
                             END_NAMESPACE(RenderSystems)