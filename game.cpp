#include "game.hpp"
#include <cmath>
#include <SDL2/SDL_image.h>

namespace game {

void Game::init(SDL_Renderer* ren) {
    renderer = ren;
    
    // Загрузка текстур
    SDL_Surface* surf = IMG_Load("assets/player.png");
    if (surf) {
        playerTex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_FreeSurface(surf);
    }
    
    surf = IMG_Load("assets/grass.png");
    if (surf) {
        bgTex = SDL_CreateTextureFromSurface(ren, surf);
        SDL_FreeSurface(surf);
    }
    
    reset();
}

void Game::reset() {
    playerX = 640; playerY = 360;
    playerAngle = 0;
    hp = PLAYER_HP_MAX;
    hitTimer = 0;
    dead = false;
    backToLobby = false;
    bullets.clear();
    camX = 0; camY = 0;
    enemy.reset();
}

void Game::update(float dt) {
    if (dead) return;
    
    // Движение
    if (controls.joyActive) {
        float dx = controls.joyStickX - controls.joyX;
        float dy = controls.joyStickY - controls.joyY;
        float len = sqrt(dx*dx + dy*dy);
        if (len > 0) {
            dx /= len; dy /= len;
            playerX += dx * 260.0f * dt;
            playerY += dy * 260.0f * dt;
            playerAngle = atan2(dy, dx) + 3.14159f/2;
            aimX = dx; aimY = dy;
        }
    }
    
    // Атака
    controls.atkPress += (controls.atkHold ? 1 : -1) * dt * 12;
    controls.atkPress = std::max(0.0f, std::min(1.0f, controls.atkPress));
    
    // Камера
    float targetX = playerX - 640;
    float targetY = playerY - 360;
    camX += (targetX - camX) * (1 - exp(-dt * 7.3f));
    camY += (targetY - camY) * (1 - exp(-dt * 7.3f));
    
    // Пули
    for (auto it = bullets.begin(); it != bullets.end();) {
        it->x += it->vx * dt;
        it->y += it->vy * dt;
        it->life -= dt;
        if (it->life <= 0) it = bullets.erase(it);
        else ++it;
    }
    
    // Враг
    enemy.update(dt, {playerX, playerY}, bullets, [this](int dmg) {
        hp -= dmg;
        hitTimer = 1;
        if (hp <= 0) {
            hp = 0;
            dead = true;
            backToLobby = true;
        }
    });
    
    hitTimer = std::max(0.0f, hitTimer - dt * 3);
}

void Game::draw(SDL_Renderer* ren) {
    // Мир (камера)
    // Здесь OpenGL или SDL_RenderCopy с камерой
    
    // Игрок
    SDL_Rect rect = {(int)(playerX - camX - 27.5f), 
                     (int)(playerY - camY - 27.5f), 55, 55};
    SDL_RenderCopyEx(ren, playerTex, nullptr, &rect, 
                     playerAngle * 180 / 3.14159f, nullptr, SDL_FLIP_NONE);
    
    // HP
    drawHPBar(ren, 1060, 30, hp, PLAYER_HP_MAX);
}

void Game::drawHPBar(SDL_Renderer* ren, int x, int y, int hp, int max) {
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 128);
    SDL_Rect bg = {x, y, 200, 18};
    SDL_RenderFillRect(ren, &bg);
    
    SDL_SetRenderDrawColor(ren, 76, 217, 89, 255);
    SDL_Rect bar = {x, y, (int)(200 * (float)hp/max), 18};
    SDL_RenderFillRect(ren, &bar);
}

void Game::spawnBullet(float dx, float dy) {
    Bullet b;
    b.x = playerX; b.y = playerY;
    b.vx = dx * BULLET_SPEED;
    b.vy = dy * BULLET_SPEED;
    bullets.push_back(b);
}

void Game::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x, y = e.button.y;
        if (x > 20 && x < 160 && y > 20 && y < 75) {
            backToLobby = true;
        }
    }
}

} // namespace game
