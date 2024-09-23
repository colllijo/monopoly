cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "communication")
set(LIBRARY_NAME "communication")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(SOURCE_DIR  "src")
set(INCLUDE_DIR "include")

# Set header files
set(HEADERS
  "${INCLUDE_DIR}/${LIBRARY_NAME}/CommunicationService.hpp"
  "${INCLUDE_DIR}/${LIBRARY_NAME}/ThreadSafeMap.hpp"
  "${INCLUDE_DIR}/${LIBRARY_NAME}/Command.hpp"
)

# Set source files
set(SOURCES
  "${SOURCE_DIR}/CommunicationService.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

target_include_directories(
  ${LIBRARY_NAME}
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR}/${LIBRARY_NAME}>
    $<INSTALL_INTERFACE:${INCLUDE_DIR}/${LIBRARY_NAME}>
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:${INCLUDE_DIR}>
)
target_link_libraries(${PROJECT_NAME} PUBLIC
  amqpcpp
  event
  nlohmann_json
)
