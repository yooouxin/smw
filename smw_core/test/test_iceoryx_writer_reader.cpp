//
// Created by youxinxin on 2021/12/5.
//
#include "iceoryx/iceoryx_reader.h"
#include "iceoryx/iceoryx_writer.h"
#include "serializer/serializer_protobuf.h"
#include "test.h"
#include "test_data.pb.h"
#include <google/protobuf/util/message_differencer.h>

using namespace smw::core;


constexpr static uint32_t TEST_SERVICE = 10;
constexpr static uint32_t TEST_INSTANCE = 1;
constexpr static uint32_t TEST_EVENT = 2;
using namespace std::chrono_literals;
static constexpr std::chrono::duration<std::int64_t, std::milli> WAIT_TIME_MS = 10ms;

TEST(iceoryx_test, write_and_read)
{
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;

    IceoryxWriter<SearchRequest, SerializerProtobuf<SearchRequest>> writer(serviceDescription, TEST_EVENT);
    IceoryxReader<SearchRequest, SerializerProtobuf<SearchRequest>> reader(serviceDescription, TEST_EVENT);

    SearchRequest input_data;
    reader.setDataCallback([&input_data](auto&& value) { input_data = *value; });
    auto output_data = writer.loan();
    output_data->set_page_number(10);
    output_data->set_query("2");
    EXPECT_TRUE(writer.write(std::move(output_data)));
    std::this_thread::sleep_for(WAIT_TIME_MS);

    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(input_data, *output_data));
}