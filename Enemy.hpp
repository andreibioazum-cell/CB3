#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <random>

class Enemy {
private:
    sf::Texture tex;
    sf::Sprite sprite;
    sf::Vector2f pos;
    float angle = 0;
    int hp = 5;
    float hitTimer = 0;
    bool alive = false;
    float respawnTimer = 0;
    float attackCooldown = 0;
    
    enum State { WANDER, CHASE, RETREAT, ATTACK };
    State state = WANDER;
    float wanderTimer = 0;
    sf::Vector2f wanderDir;
    
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
    void spawn(sf::Vector2f playerPos);
    
public:
    Enemy();
    void reset();
    void update(float dt, sf::Vector2f playerPos, 
                std::vector<struct Game::Bullet>& bullets,
                std::function<void(int)> onHit);
    void draw(sf::RenderWindow& w);
    bool isAlive() const { return alive; }
};
