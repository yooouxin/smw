

if (BUILD_TEST AND NOT GTest_FOUND)
    find_package(GTest CONFIG REQUIRED)
endif (BUILD_TEST AND NOT GTest_FOUND)


set(TEST_NAME test_smw_types)


add_executable(${TEST_NAME}
        main.cpp
        test_result.cpp)

target_link_libraries(${TEST_NAME}
        PRIVATE
        GTest::gtest
        GTest::gmock
        smw_types
        )

set_target_properties(${TEST_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD ${SMW_CXX_STANDARD}
        POSITION_INDEPENDENT_CODE ON
        )

