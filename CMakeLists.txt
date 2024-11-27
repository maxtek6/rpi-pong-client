cmake_minimum_required(VERSION 3.0)
project(rpi-pong-client)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the FlatBuffers library to our project
set(FLATBUFFERS_SRC_DIR "${CMAKE_SOURCE_DIR}/external/flatbuffers")
add_subdirectory(${FLATBUFFERS_SRC_DIR} ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build EXCLUDE_FROM_ALL)

# Add the executable target for your project
add_executable(flatbuf_sample_target src/main.cpp)

# Link against FlatBuffers
target_link_libraries(flatbuf_sample_target PRIVATE flatbuffers)

# Include directories
target_include_directories(flatbuf_sample_target PRIVATE include)

