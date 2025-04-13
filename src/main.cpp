#include "RpiPongClient.hpp" 

using namespace maxtek;

int main()
{
    try
    {
        RpiPongClient PongClient;
        PongClient.connect_to_pong_server();
        PongClient.joy_init();
        PongClient.event_loop();
    }
    catch (const std::exception &e)
    {
        std::cout << e.what();
        return 1; 
    }
    return 0;
}
