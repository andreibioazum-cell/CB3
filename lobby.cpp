#include "lobby.hpp"
#include <SDL2/SDL_ttf.h>

namespace lobby {

void Lobby::init(SDL_Renderer* ren) {
    renderer = ren;
    TTF_Init();
    font = TTF_OpenFont("assets/Fredoka-Bold.ttf", 64);
}

bool Lobby::update(float dt) {
    return startGame;
}

void Lobby::draw(SDL_Renderer* ren) {
    // Градиентный фон
    SDL_SetRenderDrawColor(ren, 115, 38, 204, 255);
    SDL_RenderFillRect(ren, nullptr);
    
    if (!font) return;
    
    // Заголовок
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surf = TTF_RenderText_Solid(font, "CUBIC BATTLE", white);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(ren, surf);
    
    SDL_Rect rect = {640 - surf->w/2, 200, surf->w, surf->h};
    SDL_RenderCopy(ren, tex, nullptr, &rect);
    
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
    
    // Кнопка Play
    SDL_SetRenderDrawColor(ren, 140, 51, 217, 255);
    SDL_Rect btn = {530, 400, 220, 75};
    SDL_RenderFillRect(ren, &btn);
    
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
    SDL_RenderDrawRect(ren, &btn);
    
    TTF_Font* smallFont = TTF_OpenFont("assets/Fredoka-Bold.ttf", 30);
    surf = TTF_RenderText_Solid(smallFont, "PLAY", white);
    tex = SDL_CreateTextureFromSurface(ren, surf);
    
    rect = {640 - surf->w/2, 420, surf->w, surf->h};
    SDL_RenderCopy(ren, tex, nullptr, &rect);
    
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
    TTF_CloseFont(smallFont);
}

void Lobby::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x = e.button.x, y = e.button.y;
        if (x > 530 && x < 750 && y > 400 && y < 475) {
            startGame = true;
        }
    }
    if (e.type == SDL_FINGERDOWN) {
        float x = e.tfinger.x * 1280;
        float y = e.tfinger.y * 720;
        if (x > 530 && x < 750 && y > 400 && y < 475) {
            startGame = true;
        }
    }
}

} // namespace lobby
