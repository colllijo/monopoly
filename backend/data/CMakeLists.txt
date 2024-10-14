cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME "DataService")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

set(SOURCE_DIR  "src")
set(INCLUDE_DIR "include")

# Set header files
set(HEADERS
  "${INCLUDE_DIR}/Database.hpp"
  "${INCLUDE_DIR}/RoomRepository.hpp"
  "${INCLUDE_DIR}/Game.hpp"
)

# Set source files
set(SOURCES
  "${SOURCE_DIR}/main.cpp"
  "${SOURCE_DIR}/Database.cpp"
  "${SOURCE_DIR}/RoomRepository.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE
  nlohmann_json
  communication
  pqxx
  pq
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
