//
// Created by youxinxin on 2021/12/2.
//
#include "dds/fastdds_reader.h"
#include "dds/fastdds_writer.h"
#include "test.h"
#include "test_data.pb.h"
#include <google/protobuf/util/message_differencer.h>

using namespace smw::core;
using namespace std::chrono_literals;
static constexpr std::chrono::duration<std::int64_t, std::milli> WAIT_TIME_MS = 10ms;

TEST(fastdds_writer, write)
{
    FastDDSWriter<SearchRequest> writer("testTopic");

    SearchRequest proto_data;
    proto_data.set_page_number(10);
    proto_data.set_query("2");
    EXPECT_TRUE(writer.write(proto_data));
}

TEST(fastdds_writer_reader, write_and_read)
{
    FastDDSWriter<SearchRequest> writer("testTopic");
    FastDDSReader<SearchRequest> reader("testTopic");

    SearchRequest input_data;
    reader.setDataCallback([&input_data](auto& value) { input_data = value; });
    SearchRequest output_data;
    output_data.set_page_number(10);
    output_data.set_query("2");
    EXPECT_TRUE(writer.write(output_data));
    std::this_thread::sleep_for(WAIT_TIME_MS);

    EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals(input_data, output_data));
}