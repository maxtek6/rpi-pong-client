# rpi-pong-client

Raspberry Pi pong client for cloud-based pong game. Requires a working Raspberry Pi Pong Server to work.
At a high-level, client will handle joystick events from player and then serialize the game input and send over to the server.

Server will update game state according to the user input.

---

## Prerequisites

Make sure you have the following installed:
cmake
g++

## Pulling RPI Pong Client and Flatbuffers submodule
```bash
git clone --recurse-submodule https://github.com/maxtek6/rpi-pong-client.git
cd rpi-pong-client
cd external/flatbuffers
git checkout v24.3.25
```
Note: Recurse flag is needed since the project requires building flatbuffers from source
Note: Due to versioning issues when installing flatbuffers from apt, checking out 24.3.25 branch and building that from source is what works for now.

## Building
1. Build flatbuffer from source (currently need to explictly build this version from source and run the built flatc compiler)
```bash
# Go to flatbuffers source directory, build flatc compiler from source and generate header file
cd <root-dir-proj>
cd external/flatbuffers
cmake -G "Unix Makefiles"
make -j
./flatc --cpp -o ../../include ../../pongdata.fbs

```
2. Building rpi-pong-client
```bash
# Build rpi client
cd <root-dir-proj>
mkdir build
cd build
cmake ..
make
```
3. Running test example
```bash
cd <root-dir-proj>
cd build
./flatbuf_sample_target
```

## Troubleshooting
```bash
./build/flatc --version 
```
