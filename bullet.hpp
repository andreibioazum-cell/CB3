#pragma once

#include <SFML/Graphics.hpp>

struct Bullet {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float life = 3.0f;
};
