
set(LIBRARY_NAME smw_types)

add_library(${LIBRARY_NAME} INTERFACE)
target_include_directories(${LIBRARY_NAME}
        INTERFACE
        include)

########## build test executables ##########
#
if (BUILD_TEST)
    add_subdirectory(test)
endif ()