

set(EXAMPLE_NAME smw_example_pub_sub)

add_executable(${EXAMPLE_NAME}_pub pub.cpp hello_wrold.pb.cc)

target_link_libraries(${EXAMPLE_NAME}_pub
        PRIVATE
        smw_core
        )

set_target_properties(${EXAMPLE_NAME}_pub PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD ${SMW_CXX_STANDARD}
        POSITION_INDEPENDENT_CODE ON
        )

add_executable(${EXAMPLE_NAME}_sub sub.cpp hello_wrold.pb.cc)

target_link_libraries(${EXAMPLE_NAME}_sub
        PRIVATE
        smw_core
        )

set_target_properties(${EXAMPLE_NAME}_sub PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD ${SMW_CXX_STANDARD}
        POSITION_INDEPENDENT_CODE ON
        )