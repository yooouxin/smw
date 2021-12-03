//
// Created by youxinxin on 2021/12/3.
//
#include "host_id.h"
#include "test.h"
#include <fstream>

using namespace smw::utils;

TEST(utils, host_id)
{
    auto id1 = getHostId();
    auto id2 = getHostId();
    EXPECT_EQ(id1, id2);
}
