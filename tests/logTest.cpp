#include <string>
#include "gtest/gtest.h"
#include "includes.h"
#include "LogManager.h"

USING_NAMESPACE(Spectre)

TEST(TestLog, test1)
{
    LOG_DEBUG("debug message");
    LOG_INFO("info message");
    LOG_WARN("warn message");
    LOG_ERROR("error message");
    LOG_DEBUG_FMT("[{}:{}]", __FILE__, __LINE__);
    LOG_INFO_FMT("[{}:{}]", __FILE__, __LINE__);
    LOG_WARN_FMT("[{}:{}]", __FILE__, __LINE__);
    LOG_ERROR_FMT("[{}:{}]", __FILE__, __LINE__);
    //EXP_CHECK(0, "exp check faild");
}
