#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include "enemy.hpp"

namespace game {

class Game {
public:
    struct Bullet {
        float x, y, vx, vy;
        float life = 3.0f;
    };
    
private:
    SDL_Renderer* renderer;
    SDL_Texture* playerTex;
    SDL_Texture* bgTex;
    
    float playerX = 640, playerY = 360;
    float playerAngle = 0;
    int hp = 5;
    float hitTimer = 0;
    bool dead = false;
    bool backToLobby = false;
    
    std::vector<Bullet> bullets;
    enemy::Enemy enemy;
    
    float camX = 0, camY = 0;
    float aimX = 0, aimY = -1;
    
    struct Controls {
        float joyX = 80, joyY = 640;
        float joyStickX = 80, joyStickY = 640;
        float atkX = 1200, atkY = 640;
        bool atkHold = false;
        bool joyActive = false;
        int joyId = -1;
        int atkId = -1;
        float atkPress = 0;
    } controls;
    
    const float PLAYER_SIZE = 55;
    const int PLAYER_HP_MAX = 5;
    const float BULLET_SPEED = 340 * 1.15f;
    
    void spawnBullet(float dx, float dy);
    void drawHPBar(SDL_Renderer* ren, int x, int y, int hp, int max);
    
public:
    void init(SDL_Renderer* ren);
    void update(float dt);
    void draw(SDL_Renderer* ren);
    void handleEvent(const SDL_Event& e);
    bool isBackToLobby() const { return backToLobby; }
    void reset();
};

} // namespace game
