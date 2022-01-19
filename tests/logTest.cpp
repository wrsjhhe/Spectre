#include <string>
#include "gtest/gtest.h"
#include "includes.h"
#include "Macro.h"

USING_NAMESPACE(Spectre::Common)

TEST(TestLog, test1)
{
    LOG_DEBUG("hello world");
    LOG_TRACE("hello world");
    LOG_INFO("hello world");
    LOG_WARN("hello world");
    LOG_ERROR("hello world");
}
