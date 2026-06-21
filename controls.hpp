#pragma once
#include <SFML/Graphics.hpp>

class Controls {
private:
    sf::Vector2f joyPos = {80, 640};
    sf::Vector2f joyStick = {80, 640};
    sf::Vector2f atkPos = {1200, 640};
    bool atkHold = false;
    bool joyActive = false;
    int joyId = -1;
    int atkId = -1;
    float atkPress = 0;
    sf::Vector2f aimDir = {0, -1};
    sf::Font font;
    
public:
    Controls();
    void init();
    void update(float dt);
    void draw(sf::RenderWindow& w);
    void handleTouchBegan(int id, sf::Vector2f pos);
    void handleTouchMoved(int id, sf::Vector2f pos);
    bool handleTouchEnded(int id, sf::Vector2f pos, sf::Vector2f& outAim);
    sf::Vector2f getMove();
    bool isAiming() const { return atkHold; }
    sf::Vector2f getAim() const { return aimDir; }
    void reset();
};
