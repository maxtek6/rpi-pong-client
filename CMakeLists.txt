cmake_minimum_required(VERSION 3.0)
project(rpi-pong-client)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the install directory of flatbuffers which is outside of current project
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../flatbuffers/install")

# Find FlatBuffers (this uses the FlatBuffersConfig.cmake from the install)
find_package(Flatbuffers REQUIRED CONFIG)

# SDL2
find_package(SDL2 REQUIRED)

# Add the executable target for your project
add_executable(rpi-pong-client src/RpiPongClient.cpp src/main.cpp)

# Include directories
target_include_directories(rpi-pong-client PRIVATE ../generated
                                                         ./include # Adding this directory for additional headers
                                                         ${CMAKE_SOURCE_DIR}/../flatbuffers/install/include)

# Link FlatBuffers and SDL
target_link_libraries(rpi-pong-client PRIVATE flatbuffers::flatbuffers)
target_link_libraries(rpi-pong-client PRIVATE SDL2)
