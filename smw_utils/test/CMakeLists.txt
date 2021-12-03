

if (BUILD_TEST AND NOT GTest_FOUND)
    find_package(GTest CONFIG REQUIRED)
endif (BUILD_TEST AND NOT GTest_FOUND)


set(TEST_NAME test_smw_utils)


add_executable(${TEST_NAME}
        main.cpp
        test_host_id.cpp)

target_link_libraries(${TEST_NAME}
        PRIVATE
        smw_utils
        GTest::gtest
        GTest::gmock
        )

set_target_properties(${TEST_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD ${SMW_CXX_STANDARD}
        POSITION_INDEPENDENT_CODE ON
        )

