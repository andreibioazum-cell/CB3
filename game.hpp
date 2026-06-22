#pragma once
#include <GLES2/gl2.h>
#include <vector>
#include "enemy.hpp"

namespace game {

struct Bullet {
    float x, y;
    float vx, vy;
    float life = 3.0f;
};

class Game {
private:
    GLuint program;
    
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
        float atkPress = 0;
    } controls;
    
    const float PLAYER_SIZE = 55;
    const int PLAYER_HP_MAX = 5;
    const float BULLET_SPEED = 340 * 1.15f;
    
    void createShaderProgram();
    void drawRect(float x, float y, float w, float h, float r, float g, float b);
    void drawHPBar(float x, float y, float w, float h, int hp, int max);
    
public:
    void init();
    void update(float dt);
    void draw();
    void handleTouch(float x, float y, bool pressed);
    bool isBackToLobby() const { return backToLobby; }
    void reset();
};

} // namespace game
