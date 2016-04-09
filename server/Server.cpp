#include <climits>
#include "Server.h"

const int SERVER_TICKRATE = 128;
const int SERVER_TICK_DELAY = 1000 / SERVER_TICKRATE;
const int ARENA_TICKRATE = 8;

using nlohmann::json;

void Server::receiveMessages() {
    Packet p;

    while (socket.receive(p)) {
        json j = p.data;
        std::string message = j["message"];

        std::cout << std::hex << p.ip << " " << p.port << std::endl;
        std::cout << p.data.dump() << std::endl;

        if (message == "connect") {
            onConnect(p);
        } else if (message == "dir") {
            onDir(p);
        }
    }
}

void Server::broadcast(nlohmann::json j) {
    for (Player &player : arena.players) {
        Packet p { player.ip, player.port, j };
        socket.send(p);
    }
}


static Snake makeSnake(int playerId) {
    Snake snake;
    snake.playerId = playerId;
    snake.dir = {1, 0};

    vec2 pos;
    for (int i = 0; i < 4; ++i) {
        Snake::Segment seg{pos};
        snake.segments.push_front(seg);
        ++pos.x;
    }

    return snake;
}

void Server::onConnect(Packet p) {
    json j = p.data;
    uint8_t r = j["color"][0];
    uint8_t g = j["color"][1];
    uint8_t b = j["color"][2];
    Color color {r, g, b};

    int playerId = getPlayerId(p.ip, p.port);
    if (playerId == 0) {
        std::vector<json> players;
        for(auto & player : arena.players) {
            Snake & snake = getPlayerSnake(player.playerId);
            json pj = {
                    {"playerId", player.playerId},
                    {"nick", player.nick},
                    {"color", snake.color.toJson()}
            };
            players.push_back(pj);
        }

        Player newPlayer;
        newPlayer.playerId = ++nextPlayerId;
        newPlayer.ip = p.ip;
        newPlayer.port = p.port;
        newPlayer.nick = j["nick"];
        arena.players.push_back(newPlayer);

        p.data = {
                {"message", "hello"},
                {"playerId", newPlayer.playerId},
                {"players", players}
        };
        socket.send(p);

        json cj = {
                {"message", "playerConnected"},
                {"playerId", newPlayer.playerId},
                {"nick", newPlayer.nick},
                {"color", color.toJson()}
        };
        broadcast(cj);

        Snake snake = makeSnake(newPlayer.playerId);
        snake.color = color;
        arena.snakes.push_back(snake);

        snakeMoved(snake);
    } else {
        std::cerr << "Player #" << playerId << " tries to connect though did not disconnect." << std::endl;
    }
}

void Server::onDir(Packet p) {
    int playerId = getPlayerId(p.ip, p.port);
    json j = p.data;
    std::vector<int> vec = j["dir"];
    int x = vec[0];
    int y = vec[1];
    vec2 dir {x, y};

    if (playerId) {
        Snake & snake = getPlayerSnake(playerId);
        snake.dir = dir;
    } else {
        std::cerr << "No player for " << std::hex << p.ip << std::endl;
    }

}

void Server::snakeMoved(const Snake &s) {
    std::vector<std::vector<int>> vec;
    for (auto seg : s.segments) {
        std::vector<int> pos {seg.pos.x, seg.pos.y};
        vec.push_back(pos);
    }

    json j = {
            {"message", "snakeMoved"},
            {"playerId", s.playerId},
            {"segments", vec}
    };
    broadcast(j);
}

int Server::getPlayerId(uint32_t ip, uint16_t port) {
    auto & players = arena.players;
    auto it = std::find_if(players.begin(), players.end(), [=](Player &player) {
        return player.ip == ip && player.port == port;
    });
    if (it == players.end()) {
        return 0;
    } else {
        return it->playerId;
    }
}
Snake & Server::getPlayerSnake(int playerId) {
    auto & snakes = arena.snakes;
    auto it = std::find_if(snakes.begin(), snakes.end(), [=](Snake & s) {
        return s.playerId == playerId;
    });
    assert(it != snakes.end());
    return *it;
}

void Server::sendMessages() {

}

Server::Server(Arena &arena) : socket(PORT), arena(arena) {}

void Server::restart() {
    arena = Arena{ARENA_WIDTH, ARENA_HEIGHT};
}

void Server::tick() {
    for (Snake &s : arena.snakes) {
        s.proceed(arena.width, arena.height);
        snakeMoved(s);
    }
}

void Server::run() {
    restart();

    int ticks = 0;
    while (ticks != INT_MAX) {
        receiveMessages();
        sendMessages();

        if(ticks % (SERVER_TICKRATE / ARENA_TICKRATE) == 0) {
            tick();
        }

        SDL_Delay(SERVER_TICK_DELAY);
        ++ticks;
    }
}