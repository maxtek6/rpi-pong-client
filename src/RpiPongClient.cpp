#include "RpiPongClient.hpp"

using namespace maxtek;
// TODO: place constants in a header file
constexpr const char *SERVER_IP_ADDR = "127.0.0.1";
constexpr const int SERVER_PORT = 8080;

RpiPongClient::RpiPongClient() : client_fd(-1),  joy_initialized(false), server_connected(false),
                                 joystick(nullptr)
{

}

int RpiPongClient::joy_init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }
    std::cout << "Initialized SDL\n";

    // Check if a joystick is connected
    if (SDL_NumJoysticks() < 1) {
        std::cerr << "No joysticks connected!" << std::endl;
        SDL_Quit();
        return -1;
    }

    joystick = SDL_JoystickOpen(0);
    if (joystick == nullptr)
    {
        std::cerr << "Couldn't open joystick! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    std::cout << "Joystick " << SDL_JoystickName(joystick) << " opened!" << std::endl;
    joy_initialized = true;

    joy_event_thread = std::thread([&] { this->process_joy_event(); });
    return 0;
}

void RpiPongClient::connect_to_pong_server()
{
    
    // Open up TCP socket to be used for connection to server
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        throw std::runtime_error("Could not open client socket\n");
        return;
    }

    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP_ADDR, &serv_addr.sin_addr) <= 0)
    {
        throw std::runtime_error("Invalid server address or address not supported\n");
        close(client_fd);
        return;
    }

    do
    {
        if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0 )
        {
            std::cout << "Connecting to Pong Server failed " << strerror(errno) << "\n";
            std::cout << "Retrying server connect\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        else
        {
            std::cout << "Succesfully connected to Pong Server\n";
            server_connected = true;
        }
    } while (!server_connected);
}

void RpiPongClient::process_joy_event()
{
    SDL_Event e;
    std::cout << "Started joy event thread\n";
    while (true)
    {
        // Check joystick axis motion
        // Send a test message TODO: Remove as this is just test
        std::cout << "Waiting for event\n";
        if (SDL_WaitEvent(&e))
        {
            std::string test_msg = "Axes event occured\n";
            if (e.type == SDL_JOYAXISMOTION) {
                if (e.jaxis.axis == 0) { // Left stick X axis
                    std::cout << "Left Stick X: " << e.jaxis.value << std::endl;
                } else if (e.jaxis.axis == 1) { // Left stick Y axis
                    std::cout << "Left Stick Y: " << e.jaxis.value << std::endl;
                }
                client_tx((const void *)test_msg.c_str(), test_msg.length(), 0);
            }

            // Check joystick button press
            if (e.type == SDL_JOYBUTTONDOWN) {
                std::cout << "Button " << (int)e.jbutton.button << " pressed!" << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    // Close joystick and quit SDL
    SDL_JoystickClose(joystick);
    std::cout << "Closing SDL Joystick";
    SDL_Quit();
}

void RpiPongClient::client_tx(const void *data, size_t data_len, int flags)
{
    ssize_t nbytes;
    nbytes = send(client_fd, data, data_len, flags);
    if (nbytes < 0)
    {
        std::cout << "Send error: " << strerror(errno) << "\n";
        return;
    }
}
