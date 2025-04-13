#ifndef RPI_PONG_CLIENT_HPP
#define RPI_PONG_CLIENT_HPP

#include <iostream>
// SDL Related headers
#include <SDL2/SDL.h>
// Networking related headers
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cerrno>
// Flatbuffers related headers
#include "flatbuffers/flatbuffers.h"
#include "pongdata_generated.h"
// Other utilities
#include <thread>
#include <chrono>
#include <exception>
#include <memory>

namespace maxtek
{
    class RpiPongClient
    {
    public:
        RpiPongClient();
        ~RpiPongClient();
        void joy_init();
        void connect_to_pong_server(int max_attempts = 5);
        void event_loop();
    private:
        int client_fd;
        bool joy_initialized;
        bool server_connected;
        std::unique_ptr<SDL_Joystick, decltype(&SDL_JoystickClose)> joystick = std::unique_ptr<SDL_Joystick, decltype(&SDL_JoystickClose)>(nullptr, SDL_JoystickClose);
        std::thread joy_event_thread;
        void process_joy_event();
        void client_tx(const void *data, size_t data_len, int flags);
        // TODO: Remove placeholder example for how to serialize pong data
        void flatbuff_example(); 
    };
}
#endif
