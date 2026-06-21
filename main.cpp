#include <SDL2/SDL.h>
#include <iostream>
#include "lobby.hpp"
#include "game.hpp"

enum GameState { STATE_LOBBY, STATE_GAME };
GameState currentState = STATE_LOBBY;

lobby::Lobby lobby;
game::Game game;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    SDL_Window* window = SDL_CreateWindow(
        "Cubic Battle 3",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1280, 720,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    lobby.init(renderer);
    game.init(renderer);
    
    bool running = true;
    Uint32 lastTime = SDL_GetTicks();
    
    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f;
        lastTime = currentTime;
        
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (currentState == STATE_LOBBY) lobby.handleEvent(event);
            else game.handleEvent(event);
        }
        
        if (currentState == STATE_LOBBY) {
            if (lobby.update(dt)) currentState = STATE_GAME;
        } else {
            game.update(dt);
            if (game.isBackToLobby()) currentState = STATE_LOBBY;
        }
        
        SDL_SetRenderDrawColor(renderer, 26, 10, 46, 255);
        SDL_RenderClear(renderer);
        
        if (currentState == STATE_LOBBY) lobby.draw(renderer);
        else game.draw(renderer);
        
        SDL_RenderPresent(renderer);
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
