#include "RpiPongClient.hpp" 

using namespace maxtek;
using namespace rpiCloudPong;

int main()
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

    RpiPongClient PongClient;
    PongClient.connect_to_pong_server();
    PongClient.joy_init();

    while (true);
    return 0;
}
