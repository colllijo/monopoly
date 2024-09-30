cmake_minimum_required(VERSION 3.22)

project(Monopoly LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Add external dependencies
set(LIBRARY_DIR "external")

set(AMQP-CPP_LINUX_TCP ON CACHE BOOL "Enable Linux TCP support for AMQP-CPP")
add_subdirectory(${LIBRARY_DIR}/AMQP-CPP)

add_subdirectory(${LIBRARY_DIR}/Crow)
add_subdirectory(${LIBRARY_DIR}/IXWebSocket)
add_subdirectory(${LIBRARY_DIR}/json)

set(EVENT__LIBRARY_TYPE STATIC CACHE STRING "Set library type to SHARED/STATIC/BOTH (default SHARED for MSVC, otherwise BOTH)" FORCE)
set(EVENT__DISABLE_BENCHMARK ON CACHE BOOL "Disable libevent benchmark" FORCE)
set(EVENT__DISABLE_TESTS ON CACHE BOOL "Disable libevent tests" FORCE)
set(EVENT__DISABLE_SAMPLES ON CACHE BOOL "Disable libevent samples" FORCE)
add_subdirectory(${LIBRARY_DIR}/libevent)

set(SKIP_BUILD_TEST ON CACHE BOOL "Skip building tests")
add_subdirectory(${LIBRARY_DIR}/libpqxx)

add_subdirectory(${LIBRARY_DIR}/libuuid-cmake)
add_subdirectory(${LIBRARY_DIR}/spdlog)

# Add communication library
add_subdirectory(communication)

# Add services
add_subdirectory(data)
add_subdirectory(rest)
