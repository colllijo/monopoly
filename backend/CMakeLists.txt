cmake_minimum_required(VERSION 3.22)

project(Monopoly LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

# Add external dependencies
set(LIBRARY_DIR "external")

set(SKIP_BUILD_TEST ON CACHE BOOL "Skip building tests")
set(AMQP-CPP_LINUX_TCP ON CACHE BOOL "Enable Linux TCP support for AMQP-CPP")

add_subdirectory(${LIBRARY_DIR}/AMQP-CPP)
add_subdirectory(${LIBRARY_DIR}/Crow)
add_subdirectory(${LIBRARY_DIR}/IXWebSocket)
add_subdirectory(${LIBRARY_DIR}/json)
add_subdirectory(${LIBRARY_DIR}/libevent)
add_subdirectory(${LIBRARY_DIR}/libpqxx)
add_subdirectory(${LIBRARY_DIR}/libuuid-cmake)

unset(SKIP_BUILD_TEST)

# Add communication library
add_subdirectory(communication)

# Add services
add_subdirectory(data)
add_subdirectory(rest)
