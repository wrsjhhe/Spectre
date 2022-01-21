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
    EXP_CHECK(0, "exp check faild");
}
