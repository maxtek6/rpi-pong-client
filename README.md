# rpi-pong-client

Raspberry Pi pong client for cloud-based pong game. Requires a working Raspberry Pi Pong Server to work.
At a high-level, client will handle joystick events from player and then serialize the game input and send over to the server.

Server will update game state according to the user input.

---

## Prerequisites

Make sure you have the following installed:
cmake
g++

## Prequisite: See readme from parent repo rpi-cloud-pong
Complete all steps in prerequisite section in README of root project
https://github.com/maxtek6/rpi-cloud-pong.git

## Building Server
```bash
cd <root-of-rpi-cloud-pong>
cd rpi-pong-client
mkdir build
cd build
cmake ..
make -j
```

## Running Programs (recommended to be ran in the following order)
## 1. Run Client
```bash
cd <root-of-rpi-cloud-pong>
cd rpi-pong-server
./build/rpi-pong-server
```

## 2. Run Server
## NOTE: A connected joystick to the machine running client is required!!
```bash
cd <root-of-rpi-cloud-pong>
cd rpi-pong-client
./build/rpi-pong-client
```
