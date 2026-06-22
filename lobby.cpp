#include "lobby.hpp"
#include <android/log.h>
#include <cmath>
#include <cstdlib>

#define LOG_TAG "Lobby"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

namespace lobby {

const char* vertexShader = 
    "attribute vec3 aPos;"
    "uniform mat4 uTransform;"
    "uniform vec4 uColor;"
    "varying vec4 vColor;"
    "void main() {"
    "   gl_Position = uTransform * vec4(aPos, 1.0);"
    "   vColor = uColor;"
    "}";

const char* fragmentShader = 
    "precision highp float;"
    "varying vec4 vColor;"
    "void main() {"
    "   gl_FragColor = vColor;"
    "}";

void Lobby::createShaderProgram() {
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShader, nullptr);
    glCompileShader(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShader, nullptr);
    glCompileShader(fs);
    
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
}

void Lobby::createRectangle() {
    float vertices[] = {
        -0.2f, -0.1f,
         0.2f, -0.1f,
         0.2f,  0.1f,
        -0.2f,  0.1f
    };
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void Lobby::init() {
    createShaderProgram();
    createRectangle();
    LOGI("Lobby initialized!");
}

bool Lobby::update(float dt) {
    return startGame;
}

void Lobby::draw() {
    glUseProgram(program);
    
    float ortho[16] = {
        2.0f/1280, 0, 0, 0,
        0, -2.0f/720, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1
    };
    
    GLint transformLoc = glGetUniformLocation(program, "uTransform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, ortho);
    
    GLint colorLoc = glGetUniformLocation(program, "uColor");
    glUniform4f(colorLoc, 140.0f/255, 51.0f/255, 217.0f/255, 1.0f);
    
    GLint posLoc = glGetAttribLocation(program, "aPos");
    glEnableVertexAttribArray(posLoc);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Lobby::handleTouch(float x, float y) {
    float nx = (x / 1280.0f) * 2 - 1;
    float ny = -(y / 720.0f) * 2 + 1;
    
    if (nx > -0.2f && nx < 0.2f && ny > -0.1f && ny < 0.1f) {
        startGame = true;
        LOGI("Play button pressed!");
    }
}

} // namespace lobby
