#include "game.hpp"
#include <cmath>
#include <android/log.h>

#define LOG_TAG "Game"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace game {

// Шейдеры
const char* vertexShader = 
    "attribute vec3 aPos;"
    "uniform mat4 uTransform;"
    "uniform vec4 uColor;"
    "varying vec4 vColor;"
    "void main() {"
    "   gl_Position = uTransform * vec4(aPos, 1.0);"
    "   vColor = uColor;"
    "}";

const char* fragmentShader = 
    "precision highp float;"
    "varying vec4 vColor;"
    "void main() {"
    "   gl_FragColor = vColor;"
    "}";

void Game::createShaderProgram() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShader, nullptr);
    glCompileShader(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShader, nullptr);
    glCompileShader(fs);
    
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Game::init() {
    createShaderProgram();
    enemy.init();
    reset();
    LOGI("Game initialized!");
}

void Game::reset() {
    playerX = 640; 
    playerY = 360;
    playerAngle = 0;
    hp = PLAYER_HP_MAX;
    hitTimer = 0;
    dead = false;
    backToLobby = false;
    bullets.clear();
    enemy.reset();
}

void Game::update(float dt) {
    if (dead) return;
    
    // Движение игрока
    if (controls.joyActive) {
        float dx = controls.joyStickX - controls.joyX;
        float dy = controls.joyStickY - controls.joyY;
        float len = sqrt(dx*dx + dy*dy);
        if (len > 0) {
            dx /= len; 
            dy /= len;
            playerX += dx * 260.0f * dt;
            playerY += dy * 260.0f * dt;
            playerAngle = atan2(dy, dx) + 3.14159f/2;
            aimX = dx; 
            aimY = dy;
        }
    }
    
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
    enemy.update(dt, playerX, playerY, bullets, [this](int dmg) {
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

void Game::drawRect(float x, float y, float w, float h, float r, float g, float b) {
    if (program == 0) return;
    
    glUseProgram(program);
    
    float ortho[16] = {
        2.0f/1280, 0, 0, 0,
        0, -2.0f/720, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1
    };
    
    GLint transformLoc = glGetUniformLocation(program, "uTransform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, ortho);
    
    GLint colorLoc = glGetUniformLocation(program, "uColor");
    glUniform4f(colorLoc, r, g, b, 1.0f);
    
    float vertices[] = {
        x-w/2, y-h/2,
        x+w/2, y-h/2,
        x+w/2, y+h/2,
        x-w/2, y+h/2
    };
    
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    GLint posLoc = glGetAttribLocation(program, "aPos");
    glEnableVertexAttribArray(posLoc);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDeleteBuffers(1, &vbo);
}

void Game::draw() {
    // Рисуем игрока (зелёный квадрат)
    drawRect(playerX - camX, playerY - camY, 55, 55, 0.3f, 0.85f, 0.35f);
    
    // Рисуем врага
    enemy.draw();
    
    // HP бар
    drawHPBar(1060, 30, 200, 18, hp, PLAYER_HP_MAX);
}

void Game::drawHPBar(float x, float y, float w, float h, int hp, int max) {
    // Фон
    drawRect(x + w/2, y + h/2, w, h, 0.0f, 0.0f, 0.0f);
    // HP
    drawRect(x + w/2 - (w/2)*(1 - (float)hp/max), y + h/2, 
             w * (float)hp/max, h, 0.3f, 0.85f, 0.35f);
}

void Game::handleTouch(float x, float y, bool pressed) {
    if (pressed) {
        // Джойстик
        if (abs(x - controls.joyX) < 45 && abs(y - controls.joyY) < 45) {
            controls.joyActive = true;
            controls.joyStickX = x;
            controls.joyStickY = y;
        }
        // Атака
        if (abs(x - controls.atkX) < 52 && abs(y - controls.atkY) < 52) {
            controls.atkHold = true;
        }
        // Back
        if (x > 20 && x < 160 && y > 20 && y < 75) {
            backToLobby = true;
        }
    } else {
        if (controls.atkHold) {
            controls.atkHold = false;
            // Создаём пулю
            Bullet b;
            b.x = playerX; 
            b.y = playerY;
            b.vx = aimX * BULLET_SPEED;
            b.vy = aimY * BULLET_SPEED;
            b.life = 3.0f;
            bullets.push_back(b);
        }
        controls.joyActive = false;
        controls.joyStickX = controls.joyX;
        controls.joyStickY = controls.joyY;
    }
}

} // namespace game
