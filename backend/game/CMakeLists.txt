cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "GameService")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(SOURCE_DIR  "src")
set(INCLUDE_DIR "include")
set(LIBRARY_DIR "../external")

# Set header files
set(HEADERS
)

# Set source files
set(SOURCES
  "${SOURCE_DIR}/main.cpp"
)

# Add libraries
set(AMQP-CPP_LINUX_TCP ON CACHE BOOL "Enable Linux TCP support for AMQP-CPP")
add_subdirectory(${LIBRARY_DIR}/AMQP-CPP)
add_subdirectory(${LIBRARY_DIR}/libevent)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE amqpcpp event)
