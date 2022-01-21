#pragma once

#include "LogManager.h"

#define LOG_DEBUG(msg)\
LogManager::GetInstance()->Debug(msg);

#define LOG_INFO(msg)\
LogManager::GetInstance()->Info(msg);

#define LOG_WARN(msg)\
LogManager::GetInstance()->Warn(msg);

#define LOG_ERROR(msg)\
LogManager::GetInstance()->Error(msg);

#define LOG_DEBUG_FMT(fmt, ...)\
LogManager::GetInstance()->DebugArgs(fmt, __VA_ARGS__);

#define LOG_INFO_FMT(fmt, ...)\
LogManager::GetInstance()->InfoArgs(fmt, __VA_ARGS__);

#define LOG_WARN_FMT(fmt, ...)\
LogManager::GetInstance()->WarnArgs(fmt, __VA_ARGS__);

#define LOG_ERROR_FMT(fmt, ...)\
LogManager::GetInstance()->ErrorArgs(fmt, __VA_ARGS__);