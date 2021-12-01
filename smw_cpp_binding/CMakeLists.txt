

set(LIBRARY_NAME smw_cpp_binding)

add_library(${LIBRARY_NAME}
        src/smwcpp_publisher.cpp
        src/smwcpp_runtime.cpp
        )

target_include_directories(${LIBRARY_NAME}
        PUBLIC
        include
        )

#
########## build test executables ##########
#
if (BUILD_TEST)
    add_subdirectory(test)
endif ()