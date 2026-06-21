#pragma once
#include <SFML/Graphics.hpp>

namespace lobby {

class Lobby {
private:
    sf::RenderWindow* window;
    sf::Font font;
    sf::Text title, subtitle, playText;
    sf::RectangleShape playBtn, gradientBg;
    bool startGame = false;
    
public:
    void init(sf::RenderWindow& win);
    bool update(float dt);
    void draw(sf::RenderWindow& w);
    void handleEvent(const sf::Event& e);
};

} // namespace lobby
