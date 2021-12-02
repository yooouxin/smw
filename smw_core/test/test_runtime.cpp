//
// Created by youxinxin on 2021/12/1.
//
#include "runtime.h"
#include "test.h"

using namespace smw::core;

TEST(runtime_test, construct)
{
    RuntimeOption option{"test"};
    auto& instance = Runtime::initRuntime(option);
    EXPECT_EQ(instance.getOption().name, "test");
    EXPECT_EQ(Runtime::getInstance().getOption().name,"test");
    EXPECT_EQ(&(Runtime::getInstance()), &instance);
}
