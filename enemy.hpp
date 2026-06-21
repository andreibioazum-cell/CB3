#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <functional>
#include <random>

namespace game {
    struct Bullet;
}

namespace enemy {

class Enemy {
private:
    SDL_Texture* tex;
    float x = 400, y = 300;
    float angle = 0;
    int hp = 5;
    float hitTimer = 0;
    bool alive = false;
    float respawnTimer = 0;
    float attackCooldown = 0;
    
    enum State { WANDER, CHASE, RETREAT, ATTACK };
    State state = WANDER;
    float wanderTimer = 0;
    float wanderDX = 0, wanderDY = 0;
    
    const float SIZE = 55;
    const float SPEED = 140;
    const float SIGHT = 650;
    const float ATTACK_RANGE = 65;
    const float KEEP_DIST = 55;
    const int MAX_HP = 5;
    const float RESPAWN_TIME = 2.0f;
    const float ATTACK_CD = 1.0f;
    const int DAMAGE = 1;
    
    std::mt19937 rng;
    void spawn(float px, float py);
    
public:
    Enemy();
    void init(SDL_Renderer* ren);
    void reset();
    void update(float dt, float px, float py, 
                std::vector<game::Bullet>& bullets,
                std::function<void(int)> onHit);
    void draw(SDL_Renderer* ren);
    bool isAlive() const { return alive; }
};

} // namespace enemy
