//
// Created by youxinxin on 2021/12/1.
//
#include "smwcpp_runtime.h"
#include "test.h"


TEST(runtime_test, construct)
{
    smw::cpp::RuntimeOption option{"test"};
    auto& instance = smw::cpp::Runtime::createInstance(option);
    EXPECT_EQ(instance.getName(), "test");

    auto& instance2 = smw::cpp::Runtime::createInstance(option);
    EXPECT_EQ(&instance,&instance2);
}
