#include "enemy.hpp"
#include <cmath>
#include <SDL2/SDL_image.h>

namespace enemy {

Enemy::Enemy() : rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    reset();
}

void Enemy::init(SDL_Renderer* ren) {
    SDL_Surface* surf = IMG_Load("assets/player.png");
    if (surf) {
        tex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_FreeSurface(surf);
    }
}

void Enemy::reset() {
    alive = false;
    respawnTimer = RESPAWN_TIME;
    hp = MAX_HP;
}

void Enemy::spawn(float px, float py) {
    float minR = std::min(1280.0f, 720.0f) * 0.30f;
    float maxR = std::min(1280.0f, 720.0f) * 0.45f;
    float a = std::uniform_real_distribution<float>(0, 6.28319f)(rng);
    float dist = minR + std::uniform_real_distribution<float>(0, 1)(rng) * (maxR - minR);
    x = px + cos(a) * dist;
    y = py + sin(a) * dist;
    alive = true;
    hp = MAX_HP;
    state = WANDER;
}

void Enemy::update(float dt, float px, float py, 
                   std::vector<game::Bullet>& bullets,
                   std::function<void(int)> onHit) {
    if (!alive) {
        respawnTimer -= dt;
        if (respawnTimer <= 0) spawn(px, py);
        return;
    }
    
    float dx = px - x;
    float dy = py - y;
    float dist = sqrt(dx*dx + dy*dy);
    if (dist > 0) { dx /= dist; dy /= dist; }
    
    if (dist < SIGHT) {
        if (dist > ATTACK_RANGE) state = CHASE;
        else if (dist < KEEP_DIST) state = RETREAT;
        else state = ATTACK;
    } else {
        state = WANDER;
    }
    
    if (state == CHASE) {
        x += dx * SPEED * dt;
        y += dy * SPEED * dt;
    } else if (state == RETREAT) {
        x -= dx * SPEED * 0.8f * dt;
        y -= dy * SPEED * 0.8f * dt;
    } else if (state == ATTACK) {
        attackCooldown -= dt;
        if (attackCooldown <= 0) {
            attackCooldown = ATTACK_CD;
            if (onHit) onHit(DAMAGE);
        }
    } else if (state == WANDER) {
        wanderTimer -= dt;
        if (wanderTimer <= 0) {
            wanderTimer = 1 + std::uniform_real_distribution<float>(0, 2)(rng);
            float a = std::uniform_real_distribution<float>(0, 6.28319f)(rng);
            wanderDX = cos(a);
            wanderDY = sin(a);
        }
        x += wanderDX * SPEED * 0.35f * dt;
        y += wanderDY * SPEED * 0.35f * dt;
    }
    
    angle = atan2(dy, dx) + 3.14159f/2;
    hitTimer = std::max(0.0f, hitTimer - dt * 3);
}

void Enemy::draw(SDL_Renderer* ren) {
    if (!alive || !tex) return;
    
    SDL_Rect rect = {(int)(x - 27.5f), (int)(y - 27.5f), 55, 55};
    SDL_RenderCopyEx(ren, tex, nullptr, &rect, 
                     angle * 180 / 3.14159f, nullptr, SDL_FLIP_NONE);
}

} // namespace enemy
