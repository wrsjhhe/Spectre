#pragma once
#include "CommonDefinitions.h"
#include <sstream>

BEGIN_NAMESPACE_SPECTRE

template <typename SSType>
void FormatStrSS(SSType& ss)
{
}

template <typename SSType, typename ArgType>
void FormatStrSS(SSType& ss, const ArgType& Arg)
{
    ss << Arg;
}

template <typename SSType, typename FirstArgType, typename... RestArgsType>
void FormatStrSS(SSType& ss, const FirstArgType& FirstArg, const RestArgsType&... RestArgs)
{
    FormatStrSS(ss, FirstArg);
    FormatStrSS(ss, RestArgs...); // recursive call using pack expansion syntax
}

template <typename... RestArgsType>
std::string FormatString(const RestArgsType&... Args)
{
    std::stringstream ss;
    FormatStrSS(ss, Args...);
    return ss.str();
}

END_NAMESPACE_SPECTRE