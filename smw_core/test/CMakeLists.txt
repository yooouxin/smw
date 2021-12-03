

if (BUILD_TEST AND NOT GTest_FOUND)
    find_package(GTest CONFIG REQUIRED)
endif (BUILD_TEST AND NOT GTest_FOUND)


set(TEST_NAME test_smw_core)


add_executable(${TEST_NAME}
        main.cpp
        test_runtime.cpp
        test_publisher.cpp
        test_subscriber.cpp
        test_fastdds_data_type_protobuf.cpp
        test_data.pb.cc
        test_fastdds_writer_reader.cpp test_service_registry.cpp)

target_link_libraries(${TEST_NAME}
        PRIVATE
        smw_core
        GTest::gtest
        GTest::gmock
        )

set_target_properties(${TEST_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD ${SMW_CXX_STANDARD}
        POSITION_INDEPENDENT_CODE ON
        )

