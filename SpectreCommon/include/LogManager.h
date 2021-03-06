#pragma once
#include "SpectreDef.h"
#include <string>
#include <memory>
#include <exception>
#include <spdlog/spdlog.h>
#include "FormotString.h"
#ifndef ROOT_PATH_SIZE
#	define ROOT_PATH_SIZE 0
#endif

#define __FILENAME__ (static_cast<const char *>(__FILE__) + ROOT_PATH_SIZE)

BEGIN_NAMESPACE_SPECTRE

class LogManager
{
public:
    static LogManager* GetInstance();
public:
    void Init(const std::string& loggerName, const std::string& filePath, const size_t size);
    void Debug(const std::string& strMsg);
    void Info(const std::string& strMsg);
    void Error(const std::string& strMsg);
    void Warn(const std::string& strMsg);

    template<typename... Args>
    void DebugArgs(const std::string& fmt, Args &&... args)
    {
        if (nullptr == m_logger)
        {
            CreateDefaultLogger();
        }

        if (nullptr != m_logger)
        {
            m_logger->debug(fmt, std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void InfoArgs(const std::string& fmt, Args &&... args)
    {
        if (nullptr == m_logger)
        {
            CreateDefaultLogger();
        }

        if (nullptr != m_logger)
        {
            m_logger->info(fmt, std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void ErrorArgs(const std::string& fmt, Args &&... args)
    {
        if (nullptr == m_logger)
        {
            CreateDefaultLogger();
        }

        if (nullptr != m_logger)
        {
            m_logger->error(fmt, std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void WarnArgs(const std::string& fmt, Args &&... args)
    {
        if (nullptr == m_logger)
        {
            CreateDefaultLogger();
        }

        if (nullptr != m_logger)
        {
            m_logger->warn(fmt, std::forward<Args>(args)...);
        }
    }
private:
    LogManager() = default;
    LogManager(const LogManager& rhs) = delete;
    LogManager& operator = (const LogManager& rhs) = delete;
    void CreateDefaultLogger();
private:
    std::shared_ptr<spdlog::logger> m_logger;
};

#define LOG_DEBUG(msg)\
LogManager::GetInstance()->Debug(FormatString(msg));

#define LOG_INFO(msg)\
LogManager::GetInstance()->Info(FormatString(msg));

#define LOG_WARN(msg)\
LogManager::GetInstance()->Warn(FormatString(msg));

#define LOG_ERROR(msg)\
LogManager::GetInstance()->Error(FormatString(msg));

#define LOG_DEBUG_FMT(fmt, ...)\
LogManager::GetInstance()->DebugArgs(fmt, __VA_ARGS__);

#define LOG_INFO_FMT(fmt, ...)\
LogManager::GetInstance()->InfoArgs(fmt, __VA_ARGS__);

#define LOG_WARN_FMT(fmt, ...)\
LogManager::GetInstance()->WarnArgs(fmt, __VA_ARGS__);

#define LOG_ERROR_FMT(fmt, ...)\
LogManager::GetInstance()->ErrorArgs(fmt, __VA_ARGS__);

#define EXP_CHECK(x,msg)												 \
	do																	 \
	{																	 \
		if (!(x))														 \
		{																 \
			LOG_ERROR_FMT("[{}:{}] {}", __FILENAME__, __LINE__, fmt::format(msg));	\
			throw std::exception("exp check faild");					 \
		}																 \
	} while (0);														 \

END_NAMESPACE_SPECTRE

