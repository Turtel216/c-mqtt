cmake_minimum_required(VERSION 3.14)
project(broker C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build tests" ON)

# Enable compiler warnings and useful flags
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        -Wall
        -Wextra
        # -Wpedantic
        # -Werror
        # -Wshadow
        # -Wformat=2
        # -Wfloat-equal
        # -Wcast-align
        # -Wconversion
        # -Wnull-dereference
        # -fno-common
        # -fstack-protector-strong
        # -D_FORTIFY_SOURCE=2
    )
endif()

# Find required packages (optional, depending on the broker)
# find_package(OpenSSL REQUIRED) # Uncomment if using SSL

# Broker source files
file(GLOB_RECURSE BROKER_SRC CONFIGURE_DEPENDS src/*.c)
add_library(broker_lib STATIC ${BROKER_SRC})
add_executable(cmqtt src/main.c)
target_link_libraries(cmqtt PRIVATE broker_lib)
target_include_directories(broker_lib PUBLIC src)
target_include_directories(cmqtt PRIVATE src)

# target_link_libraries(mqtt_broker PRIVATE OpenSSL::SSL)

# Testing setup
if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
