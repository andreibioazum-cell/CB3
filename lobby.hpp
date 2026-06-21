#pragma once
#include <SDL2/SDL.h>
#include <string>

namespace lobby {

class Lobby {
private:
    SDL_Renderer* renderer;
    bool startGame = false;
    
    // Шрифт через SDL_ttf
    TTF_Font* font = nullptr;
    
public:
    void init(SDL_Renderer* ren);
    bool update(float dt);
    void draw(SDL_Renderer* ren);
    void handleEvent(const SDL_Event& e);
};

} // namespace lobby
