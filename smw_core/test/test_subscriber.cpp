//
// Created by youxinxin on 2021/12/2.
//
#include "subscriber.h"
#include "test.h"


TEST(subscriber_test, constrcut)
{
    smw::core::Subscriber<int> subscriber_topic("testTopic");
    EXPECT_EQ(subscriber_topic.getTopicName(), "testTopic");

    smw::core::Subscriber<int> subscriber_service{1, 2, 3};
    EXPECT_EQ(subscriber_service.getTopicName(), "/1/2/3");
}


TEST(subscriber_test, take_samples)
{
    smw::core::Subscriber<int> subscriber_topic("testTopic");
    EXPECT_EQ(subscriber_topic.getNewSamples().getError(), smw::core::SubscriberError::NO_IMPLEMENT);
}