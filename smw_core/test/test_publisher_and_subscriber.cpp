//
// Created by youxinxin on 2021/12/5.
//
#include "publisher.h"
#include "service_skeleton.h"
#include "subscriber.h"
#include "test.h"

using namespace smw::core;

constexpr static uint32_t TEST_SERVICE = 10;
constexpr static uint32_t TEST_INSTANCE = 1;
constexpr static uint32_t TEST_EVENT = 2;
using namespace std::chrono_literals;
static constexpr std::chrono::duration<std::int64_t, std::milli> WAIT_TIME_MS = 10ms;

TEST(pub_sub, by_ref)
{
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;

    /// request and find service by myself
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription);
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::FIND, serviceDescription);

    Publisher<SearchRequest> publisher{serviceDescription, TEST_EVENT};
    Subscriber<SearchRequest> subscriber{serviceDescription, TEST_EVENT};

    SearchRequest input_data;
    subscriber.registerReceiveHandler(
        [&input_data](SamplePtr<const SearchRequest> sample_ptr) { input_data = *sample_ptr; });

    SearchRequest output_data;
    output_data.set_page_number(10);
    output_data.set_query("Hello");
    EXPECT_FALSE(publisher.publish(output_data).hasError());

    std::this_thread::sleep_for(WAIT_TIME_MS);

    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(input_data, output_data));
}


TEST(pub_sub, by_loan)
{
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;

    /// request and find service by myself
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription);
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::FIND, serviceDescription);

    Publisher<SearchRequest> publisher{serviceDescription, TEST_EVENT};
    Subscriber<SearchRequest> subscriber{serviceDescription, TEST_EVENT};

    SearchRequest input_data;
    subscriber.registerReceiveHandler(
        [&input_data](SamplePtr<const SearchRequest> sample_ptr) { input_data = *sample_ptr; });


    SearchRequest output_data;
    output_data.set_page_number(10);
    output_data.set_query("2");

    auto output_sample_result = publisher.loanSample();
    auto output_sample = std::move(output_sample_result.getValue());
    *output_sample = output_data;
    EXPECT_FALSE(publisher.publish(std::move(output_sample)).hasError());

    std::this_thread::sleep_for(WAIT_TIME_MS);

    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(input_data, output_data));
}