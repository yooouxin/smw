
set(LIBRARY_NAME smw_utils)

add_library(${LIBRARY_NAME}
        src/host_id.cpp
        )


target_include_directories(${LIBRARY_NAME}
        PUBLIC
        include
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