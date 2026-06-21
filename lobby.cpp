#include "lobby.hpp"

void Lobby::init(sf::RenderWindow& win) {
    window = &win;
    font.loadFromFile("assets/Fredoka-Bold.ttf");
    
    title.setFont(font);
    title.setString("CUBIC BATTLE");
    title.setCharacterSize(64);
    title.setFillColor(sf::Color::White);
    title.setPosition(640 - title.getLocalBounds().width/2, 200);
    
    subtitle.setFont(font);
    subtitle.setString("Touch & Dodge");
    subtitle.setCharacterSize(22);
    subtitle.setFillColor(sf::Color::White);
    subtitle.setPosition(640 - subtitle.getLocalBounds().width/2, 280);
    
    playBtn.setSize({220, 75});
    playBtn.setFillColor(sf::Color(140, 51, 217));
    playBtn.setPosition(640 - 110, 400);
    playBtn.setOutlineThickness(3);
    playBtn.setOutlineColor(sf::Color::Black);
    
    playText.setFont(font);
    playText.setString("PLAY");
    playText.setCharacterSize(30);
    playText.setFillColor(sf::Color::White);
    playText.setPosition(640 - playText.getLocalBounds().width/2, 422);
    
    gradientBg.setSize({1280, 720});
    gradientBg.setFillColor(sf::Color(115, 38, 204));
}

bool Lobby::update(float dt) {
    return startGame;
}

void Lobby::draw(sf::RenderWindow& w) {
    w.draw(gradientBg);
    w.draw(title);
    w.draw(subtitle);
    w.draw(playBtn);
    w.draw(playText);
}

void Lobby::handleEvent(const sf::Event& e) {
    if (e.type == sf::Event::TouchBegan || e.type == sf::Event::MouseButtonPressed) {
        sf::Vector2f pos;
        if (e.type == sf::Event::TouchBegan) pos = {e.touch.x, e.touch.y};
        else pos = {e.mouseButton.x, e.mouseButton.y};
        
        if (pos.x > 530 && pos.x < 750 && pos.y > 400 && pos.y < 475) {
            startGame = true;
        }
    }
}
