#ifndef STEROIDS_SERVER_H
#define STEROIDS_SERVER_H


#include "../common/Socket.h"
#include "../common/Arena.h"
#include "../common/ServerConfig.h"

using nlohmann::json;

class Server {
private:

    Socket socket;
    Arena &arena;
    int nextPlayerId = 0;
    ServerConfig config;

    void receiveMessages();

	void addPoints(const Fruit&f, const Snake&s);

    void broadcast(const std::string &data);

    void onConnect(Packet p);
	
    void sendFruits();

    void broadcastSnapshot();

    int getPlayerId(uint32_t ip, uint16_t port);

    Snake & getPlayerSnake(int playerId);

    void onDir(Packet packet);

    void moveSnakes();

	Player & getPlayerByID(int playerId);

    void handleCollisions();

    void handleFruits();

    void handleEating(Snake &snake, Fruit &fruit);

    void killSnake(Snake &snake);

    void handleSnakesCollision(Snake &a, Snake &b, std::vector<Snake *> & vector);
    
    void spawnFruit();

public:

    static const auto ARENA_WIDTH = 17; // segments
    static const auto ARENA_HEIGHT = 17; // segments
    static const int PORT = 28666;

    Server(Arena &arena, ServerConfig cfg);

    void restart();

    void tick();

    void run();
};


#endif //STEROIDS_SERVER_H
