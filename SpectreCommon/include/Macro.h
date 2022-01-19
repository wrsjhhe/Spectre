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