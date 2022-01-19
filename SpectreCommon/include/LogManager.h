#pragma once
#include "CommonDefinitions.h"
#include <string>
#include <memory>

namespace spdlog
{
    class logger;
}

BEGIN_NAMESPACE_COMMON

class LogManager
{
public:
    static LogManager* GetInstance();
public:
    void Init(const std::string& loggerName, const std::string& filePath, const size_t size);
    void Info(const std::string& strMsg);
    void Error(const std::string& strMsg);
    void Warn(const std::string& strMsg);
private:
    LogManager() = default;
    LogManager(const LogManager& rhs) = delete;
    LogManager& operator = (const LogManager& rhs) = delete;
    void CreateDefaultLogger();
private:
    std::shared_ptr<spdlog::logger> m_logger;
};

END_NAMESPACE_COMMON