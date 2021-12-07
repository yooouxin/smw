//
// Created by youxinxin on 2021/12/2.
//
#include "service_description.h"
#include "subscriber.h"
#include "test.h"

using namespace smw::core;
constexpr static uint32_t TEST_SERVICE = 10;
constexpr static uint32_t TEST_INSTANCE = 1;
constexpr static uint32_t TEST_EVENT = 2;


TEST(subscriber_test, constrcut)
{
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;
    smw::core::Subscriber<SearchRequest> subscriber{serviceDescription, TEST_EVENT};
    EXPECT_EQ(subscriber.getTopicName(), fmt::format("/{}/{}/{}", TEST_SERVICE, TEST_INSTANCE, TEST_EVENT));
}
