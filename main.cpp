 #include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <jni.h>
#include <android/log.h>
#include "lobby.hpp"
#include "game.hpp"

#define LOG_TAG "CubicBattle"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

enum GameState { STATE_LOBBY, STATE_GAME };
GameState currentState = STATE_LOBBY;

lobby::Lobby lobby;
game::Game game;

// ============================================================
// JNI ФУНКЦИЯ ДЛЯ ЗАПУСКА ИГРЫ ИЗ JAVA
// ============================================================

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeRun(JNIEnv* env, jobject obj, jobject surface) {
    
    // Создаём окно для Android
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
}
