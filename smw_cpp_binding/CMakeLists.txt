

set(LIBRARY_NAME smw_cpp_binding)

add_library(${LIBRARY_NAME} SHARED src/smwcpp_publisher.cpp)

target_include_directories(${LIBRARY_NAME}
        PRIVATE
        include
        )

add_executable(test_${LIBRARY_NAME} test/test_cpp_binding.cpp)

target_include_directories(test_${LIBRARY_NAME}
        PRIVATE
        include
        )