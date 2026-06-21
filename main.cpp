#include <SFML/Graphics.hpp>
#include "lobby.hpp"
#include "game.hpp"

enum GameState { STATE_LOBBY, STATE_GAME };
GameState currentState = STATE_LOBBY;

lobby::Lobby lobby;
game::Game game;

int main() {
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Cubic Battle 3");
    window.setFramerateLimit(60);
    
    lobby.init(window);
    game.init(window);
    
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;
        
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (currentState == STATE_LOBBY) lobby.handleEvent(event);
            else game.handleEvent(event);
        }
        
        if (currentState == STATE_LOBBY) {
            if (lobby.update(dt)) currentState = STATE_GAME;
        } else {
            game.update(dt);
            if (game.isBackToLobby()) currentState = STATE_LOBBY;
        }
        
        window.clear();
        if (currentState == STATE_LOBBY) lobby.draw(window);
        else game.draw(window);
        window.display();
    }
    return 0;
}
