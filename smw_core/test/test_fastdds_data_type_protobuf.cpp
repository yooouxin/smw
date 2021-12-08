//
// Created by youxinxin on 2021/12/2.
//
#include "dds/fastdds_data_type.h"
#include "serializer/serializer_protobuf.h"
#include "test.h"
#include "test_data.pb.h"
#include <google/protobuf/util/message_differencer.h>

using namespace smw::core;

TEST(fastdds_data_type_protobuf, construct)
{
    FastDDSDataType<SearchRequest, SerializerProtobuf<SearchRequest>> test;
    EXPECT_EQ(test.getName(), SearchRequest().GetTypeName());

    SearchRequest proto_data;
    proto_data.set_page_number(1);
    proto_data.set_query("HHH");
    auto proto_size = proto_data.ByteSizeLong();

    std::vector<std::byte> proto_buffer(test.getSerializedSizeProvider(&proto_data)());

    EXPECT_TRUE(proto_data.SerializePartialToArray(proto_buffer.data(), proto_buffer.size()));

    eprosima::fastrtps::rtps::SerializedPayload_t payload(proto_buffer.size());
    EXPECT_TRUE(test.serialize(&proto_data, &payload));
    /// actual dds payload length has added  4 bytes filed to indicate length of actual data
    //    EXPECT_EQ(payload.length, proto_size + 4);

    auto* dds_data = test.createData();
    EXPECT_TRUE(test.deserialize(&payload, dds_data));
    EXPECT_TRUE(
        google::protobuf::util::MessageDifferencer::Equals(proto_data, *(reinterpret_cast<SearchRequest*>(dds_data))));

    test.deleteData(dds_data);
}