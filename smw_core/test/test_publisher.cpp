//
// Created by youxinxin on 2021/12/2.
//
#include "publisher.h"
#include "service_description.h"
#include "test.h"

using namespace smw::core;
constexpr static uint32_t TEST_SERVICE = 10;
constexpr static uint32_t TEST_INSTANCE = 1;
constexpr static uint32_t TEST_EVENT = 2;
TEST(publisher_test, construct)
{
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;
    smw::core::Publisher<SearchRequest> publisher_service{serviceDescription, TEST_EVENT};
    EXPECT_EQ(publisher_service.getTopicName(), fmt::format("/{}/{}/{}", TEST_SERVICE, TEST_INSTANCE, TEST_EVENT));
}

TEST(publisher_test, loan)
{
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;
    smw::core::Publisher<SearchRequest> publisher_service{serviceDescription, TEST_EVENT};

    EXPECT_FALSE(publisher_service.loanSample().hasError());
    SearchRequest request;
    EXPECT_FALSE(publisher_service.publish(request).hasError());
}