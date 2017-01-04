#pragma once

#include <string>

#include "Common/Useful.hpp"
#include "Common/NetworkConst.hpp"
#include "World/Camera.hpp"
#include "PlayerCommand.hpp"

using std::string;

class Server;
class Game;
class NetworkController;
struct ServerCommand;

class Mob;

class Player {
private:
    string ckey;
    bool connected;
    Game *game;

    Mob *mob;
    uptr<Camera> camera;

    ThreadSafeQueue<ServerCommand *> commandsToClient;
    ThreadSafeQueue<PlayerCommand *> commandsFromClient;

public:
    Player();

    /// Client interface
    void Authorize(string login, string password);
    void Register(string login, string password);
    void UpdateServerList();
    void JoinToGame(int id);
    void ChatMessage(std::string &message);

    void Move(Global::Direction);
    ///

    void Update();
    void SendGraphicsUpdates();

    string GetCKey() { return ckey; }

    void SetMob(Mob *mob);
    void SetCamera(Camera *camera) { this->camera.reset(camera); }

    Mob *GetMob() { return mob; }
    Camera *GetCamera() { return camera.get(); }
    bool IsConnected() { return connected; }

    void AddCommandToClient(ServerCommand *);
    void AddCommandFromClient(PlayerCommand *);

    friend NetworkController;
    friend Server;
};