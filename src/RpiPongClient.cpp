#include "RpiPongClient.hpp"

using namespace maxtek;
// For generated flatbuffer namespace usage
using namespace rpiCloudPong;
// TODO: place constants in a header file
constexpr const char *SERVER_IP_ADDR = "127.0.0.1";
constexpr const int SERVER_PORT = 8080;

RpiPongClient::RpiPongClient() : client_fd(-1),  joy_initialized(false), server_connected(false)
{

}

RpiPongClient::~RpiPongClient()
{
    Uint32 initialized;
    if (client_fd > 0)
    {
        std::cout << "RpiPongClient destructor closing client_fd " << client_fd << "\n";
        close(client_fd);
    }

    // Explicitly call joystick custom deleter to avoid subsytem deinit and 
    // resource clean-up ordering issue
    joystick.reset();
    // we can call SDL_Quit() here regardless of what was initialized
    SDL_Quit();
    std::cout << "RpiPongClient destructor succesfully completed\n";
}

void RpiPongClient::joy_init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) < 0) {
        throw std::runtime_error(std::string("SDL could not initialize! SDL_Error: ") + SDL_GetError() + "\n");
    }
    std::cout << "Initialized SDL\n";

    // Check if a joystick is connected
    if (SDL_NumJoysticks() < 1) {
        throw std::runtime_error("No joysticks connected!\n");
    }

    joystick.reset(SDL_JoystickOpen(0));
    if (nullptr == joystick.get())
    {
        throw std::runtime_error(std::string("Couldn't open joystick! SDL_Error: ") + SDL_GetError() + "\n");
    }

    std::cout << "Joystick " << SDL_JoystickName(joystick.get()) << " opened" << std::endl;
    joy_initialized = true;
    joy_event_thread = std::thread([&] { this->process_joy_event(); });
}

void RpiPongClient::connect_to_pong_server(int max_attempts)
{ 
    // Open up TCP socket to be used for connection to server
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        throw std::runtime_error(std::string("Could not open client socket ") + strerror(errno) + "\n");
    }

    sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP_ADDR, &serv_addr.sin_addr) <= 0)
    {
        close(client_fd);
        throw std::runtime_error(std::string("inet_pton error: ") + strerror(errno) + "\n");
    }

    int attempts = 0;
    while(!server_connected && attempts < max_attempts)
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
        attempts++;
    }

    if (!server_connected)
    {
        close(client_fd);
        throw std::runtime_error(std::string("Could not connect to Pong Server after ") + std::to_string(max_attempts) + " attempts\n");
    }
}

void RpiPongClient::event_loop(void)
{
    // TODO: shutdown flag that is associated with a CV
    // If something goes wrong after initialization, shutdown flag should be set and cv should be triggered
    while (true); 
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
            if (e.type == SDL_JOYAXISMOTION)
            {
                // TODO: move this into main thread and push both axis values to a queue
                if (e.jaxis.axis == 0)
                {   // Left stick X axis
                    std::cout << "Left Stick X: " << e.jaxis.value << std::endl;
                }
                else if (e.jaxis.axis == 1)
                {   // Left stick Y axis
                    std::cout << "Left Stick Y: " << e.jaxis.value << std::endl;
                }
                client_tx((const void *)test_msg.c_str(), test_msg.length(), 0);
            }

            // Check joystick button press
            if (e.type == SDL_JOYBUTTONDOWN)
            {
                std::cout << "Button " << (int)e.jbutton.button << " pressed!" << std::endl;
            }
        }
        else
        {
            // Set shutdown flag to true, cv notify
            throw std::runtime_error(std::string("SDL_WaitEvent error: ") + SDL_GetError() + "\n");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void RpiPongClient::client_tx(const void *data, size_t data_len, int flags)
{
    ssize_t nbytes;
    nbytes = send(client_fd, data, data_len, flags);
    if (nbytes < 0)
    {
        throw std::runtime_error(std::string("Send error: ") +  strerror(errno) + "\n");
    }
}

// TODO: To be used as a basis for serializing and deserialiing buffer
void RpiPongClient::flatbuff_example(void)
{
    // Create a FlatBufferBuilder.
    flatbuffers::FlatBufferBuilder builder;

    // Create paddle properties
    auto paddle1 = Createpaddle_properties(builder, 10, 20);
    auto paddle2 = Createpaddle_properties(builder, 30, 40);

    // Create ball properties
    auto ball = Createball_properties(builder, 100, 150);

    // Create vectors for paddles
    std::vector<flatbuffers::Offset<paddle_properties>> paddles = {paddle1, paddle2};
    auto paddles_vec = builder.CreateVector(paddles);

    // Create the game properties
    auto game = Creategame_properties(builder, paddles_vec, ball);
    builder.Finish(game);

    // Access the serialized data
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize();

    // Deserialize the data into a game_properties object
    auto game_data = Getgame_properties(buf);

    // Access paddles and ball properties
    for (auto paddle : *game_data->paddles()) {
        printf("Paddle X: %d, Y: %d\n", paddle->x_pos(), paddle->y_pos());
    }

    printf("Ball X: %d, Y: %d\n", game_data->ball()->x_pos(), game_data->ball()->y_pos());
}
