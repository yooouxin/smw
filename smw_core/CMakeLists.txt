

find_package(fmt 8.0.1 CONFIG REQUIRED)
find_package(fastrtps 2.5.0 CONFIG REQUIRED)
find_package(Protobuf 3.5.0 REQUIRED)

set(LIBRARY_NAME smw_core)

add_library(${LIBRARY_NAME}
        src/runtime.cpp
        src/service_skeleton.cpp
        src/fastdds_participant.cpp
        src/service_registry.cpp
        proto/service_info.pb.cc
        )

target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        fmt::fmt
        smw_types
        smw_utils
        fastrtps
        ${PROTOBUF_LIBRARIES}
        )

target_include_directories(${LIBRARY_NAME}
        PUBLIC
        include
        proto
        )


set_target_properties(${LIBRARY_NAME} PROPERTIES
        CXX_STANDARD_REQUIRED ON
        CXX_STANDARD ${SMW_CXX_STANDARD}
        POSITION_INDEPENDENT_CODE ON
        )
#
########## build test executables ##########
#
if (BUILD_TEST)
    add_subdirectory(test)
endif ()