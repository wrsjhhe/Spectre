#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "LogManager.h"

USING_NAMESPACE(Spectre)

LogManager* LogManager::GetInstance()
{
    static LogManager logManager;
    return &logManager;
}

void LogManager::Init(const std::string& loggerName, const std::string& filePath, const size_t size)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath, size, 3);
    file_sink->set_level(spdlog::level::trace);

    m_logger = std::shared_ptr<spdlog::logger>(new spdlog::logger(loggerName, { console_sink, file_sink }));
    m_logger->set_level(spdlog::level::debug);
}

void LogManager::Debug(const std::string& strMsg)
{
    if (nullptr == m_logger)
    {
        CreateDefaultLogger();
    }

    if (nullptr != m_logger)
    {
        m_logger->debug(strMsg);
    }
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
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);

    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/log.txt", 1048576 * 5, 3);
    file_sink->set_level(spdlog::level::trace);

    m_logger = std::shared_ptr<spdlog::logger>(new spdlog::logger("Spectre", { console_sink, file_sink }));
    m_logger->set_level(spdlog::level::debug);
}
