//
// Created by youxinxin on 2021/12/2.
//
#include "result.h"
#include "test.h"

enum TestError
{
    TEST_ERROR,
    TEST_ERROR2
};

TEST(Result, Ok)
{
    smw::types::Result<int, TestError> result(smw::types::Ok(1));
    EXPECT_DEATH(result.getError(), "");

    EXPECT_EQ(result.getValue(), 1);
    int val = -1;
    result.andThen([&](auto& value) { val = value; });

    EXPECT_EQ(val, 1);
}

TEST(Result, Err)
{
    smw::types::Result<int, TestError> result{smw::types::Err(TEST_ERROR)};
    EXPECT_DEATH(result.getValue(), "");

    EXPECT_EQ(result.getError(), TEST_ERROR);
    TestError err = TEST_ERROR2;
    result.orElse([&](auto& error) { err = error; });

    EXPECT_EQ(err, TEST_ERROR);
}
