cmake_minimum_required(VERSION 3.2)

project(smw)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

include(${CMAKE_CURRENT_SOURCE_DIR}/build_options.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/project_settings.cmake)

add_subdirectory(third_party/fmt-8.0.1)
add_subdirectory(smw_types)
add_subdirectory(smw_core)
add_subdirectory(smw_utils)