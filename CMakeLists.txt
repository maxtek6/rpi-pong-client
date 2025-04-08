cmake_minimum_required(VERSION 3.0)
project(rpi-pong-client)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the FlatBuffers library to our project
set(FLATBUFFERS_SRC_DIR "${CMAKE_SOURCE_DIR}/external/flatbuffers")
add_subdirectory(${FLATBUFFERS_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build EXCLUDE_FROM_ALL)

# SDL2
find_package(SDL2 REQUIRED)

# Add the executable target for your project
add_executable(flatbuf_sample_target src/RpiPongClient.cpp src/main.cpp)

# Link against FlatBuffers
target_link_libraries(flatbuf_sample_target PRIVATE flatbuffers)
target_link_libraries(flatbuf_sample_target PRIVATE SDL2)

# Include directories
target_include_directories(flatbuf_sample_target PRIVATE include)

# Include directories
target_include_directories(flatbuf_sample_target
    PRIVATE
        include
        src/include # Adding this directory for additional headers
)
