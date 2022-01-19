#include "LogManager.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

USING_NAMESPACE(Spectre::Common)

LogManager* LogManager::GetInstance()
{
    static LogManager logManager;
    return &logManager;
}

void LogManager::Init(const std::string& loggerName, const std::string& filePath, const size_t size)
{
    m_logger = spdlog::rotating_logger_mt(loggerName, filePath, size, 3);
}

void LogManager::Info(const std::string& strMsg)
{
    if (nullptr == m_logger)
    {
        CreateDefaultLogger();
    }

    if (nullptr != m_logger)
    {
        m_logger->info(strMsg);
    }
}

void LogManager::Error(const std::string& strMsg)
{
    if (nullptr == m_logger)
    {
        CreateDefaultLogger();
    }

    if (nullptr != m_logger)
    {
        m_logger->error(strMsg);
    }
}

void LogManager::Warn(const std::string& strMsg)
{
    if (nullptr == m_logger)
    {
        CreateDefaultLogger();
    }

    if (nullptr != m_logger)
    {
        m_logger->warn(strMsg);
    }
}

void LogManager::CreateDefaultLogger()
{
    // 5M
    m_logger = spdlog::rotating_logger_mt("Spectre", "log/log.txt", 1048576 * 5, 3);
}
