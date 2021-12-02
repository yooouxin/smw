//
// Created by youxinxin on 2021/12/2.
//
#include "dds/fastdds_data_type_protobuf.h"
#include "test.h"
#include "test_data.pb.h"
#include <google/protobuf/util/message_differencer.h>
using namespace smw::core;

TEST(fastdds_data_type_protobuf, construct)
{
    FastDDSDataTypeProtobuf<SearchRequest> test;
    EXPECT_EQ(test.getName(), SearchRequest().GetTypeName());

    SearchRequest proto_data;
    proto_data.set_page_number(1);
    proto_data.set_query("HHH");

    auto proto_size = proto_data.ByteSizeLong();
    std::vector<std::byte> proto_buffer(proto_size);

    proto_data.SerializePartialToArray(proto_buffer.data(), proto_buffer.size());

    eprosima::fastrtps::rtps::SerializedPayload_t payload(proto_size);
    EXPECT_TRUE(test.serialize(&proto_data, &payload));
    EXPECT_EQ(payload.length, proto_size);

    auto* dds_data = test.createData();
    EXPECT_TRUE(test.deserialize(&payload, dds_data));
    EXPECT_TRUE(
        google::protobuf::util::MessageDifferencer::Equals(proto_data, *(reinterpret_cast<SearchRequest*>(dds_data))));

    test.deleteData(dds_data);
}