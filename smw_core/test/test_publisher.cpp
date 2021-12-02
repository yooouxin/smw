//
// Created by youxinxin on 2021/12/2.
//
#include "publisher.h"
#include "test.h"


TEST(publisher_test, construct)
{
    smw::core::Publisher<int> publisher_topic{"testTopic"};
    EXPECT_EQ(publisher_topic.getTopicName(), "testTopic");

    smw::core::Publisher<int> publisher_service{1, 2, 3};
    EXPECT_EQ(publisher_service.getTopicName(), "/1/2/3");
}

TEST(publisher_test, loan)
{
    smw::core::Publisher<int> publisher_topic{"testTopic"};
    EXPECT_EQ(publisher_topic.loanSample().getError(), smw::core::PublisherError::NO_IMPLEMENT);
    EXPECT_EQ(publisher_topic.publish(1).getError(), smw::core::PublisherError::NO_IMPLEMENT);
}