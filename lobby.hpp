#pragma once
#include <GLES2/gl2.h>
#include <string>

namespace lobby {

class Lobby {
private:
    bool startGame = false;
    
    // OpenGL ресурсы
    GLuint program;
    GLuint vbo;
    GLuint texture;
    int screenWidth = 1280;
    int screenHeight = 720;
    
    void createShaderProgram();
    void createRectangle();
    
public:
    void init();
    bool update(float dt);
    void draw();
    void handleTouch(float x, float y);
    bool isStartGame() const { return startGame; }
    void reset() { startGame = false; }
};

} // namespace lobby
