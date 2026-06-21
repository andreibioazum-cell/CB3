#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "enemy.hpp"

namespace game {

class Game {
public:
    struct Bullet {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float life = 3.0f;
    };
    
private:
    sf::RenderWindow* window;
    sf::Texture playerTex, bgTex;
    sf::Sprite player, bg;
    sf::Font font;
    sf::Text hpText;
    
    sf::Vector2f playerPos = {640, 360};
    float playerAngle = 0;
    int hp = 5;
    float hitTimer = 0;
    bool dead = false;
    bool backToLobby = false;
    
    std::vector<Bullet> bullets;
    enemy::Enemy enemy;
    
    sf::Vector2f cam = {0, 0};
    sf::Vector2f aimDir = {0, -1};
    
    struct Controls {
        sf::Vector2f joyPos = {80, 640};
        sf::Vector2f joyStick = {80, 640};
        sf::Vector2f atkPos = {1200, 640};
        bool atkHold = false;
        bool joyActive = false;
        int joyId = -1;
        int atkId = -1;
        float atkPress = 0;
    } controls;
    
    const float PLAYER_SIZE = 55;
    const int PLAYER_HP_MAX = 5;
    const float BULLET_SPEED = 340 * 1.15f;
    
    void spawnBullet(sf::Vector2f dir);
    void drawHPBar(sf::RenderWindow& w, sf::Vector2f pos, int hp, int max);
    void drawControls(sf::RenderWindow& w);
    
public:
    void init(sf::RenderWindow& win);
    void update(float dt);
    void draw(sf::RenderWindow& w);
    void handleEvent(const sf::Event& e);
    bool isBackToLobby() const { return backToLobby; }
    void reset();
};

} // namespace game
