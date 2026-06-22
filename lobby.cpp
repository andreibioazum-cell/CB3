#include "lobby.hpp"
#include <android/log.h>
#include <cmath>
#include <cstdlib>

#define LOG_TAG "Lobby"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)  // <-- ДОБАВИТЬ ЭТУ СТРОКУ

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
    LOGI("createShaderProgram: Creating vertex shader...");
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShader, nullptr);
    glCompileShader(vs);
    
    GLint compiled;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(infoLen);
            glGetShaderInfoLog(vs, infoLen, nullptr, infoLog);
            LOGE("Vertex shader compile error: %s", infoLog);
            free(infoLog);
        }
    }
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShader, nullptr);
    glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(infoLen);
            glGetShaderInfoLog(fs, infoLen, nullptr, infoLog);
            LOGE("Fragment shader compile error: %s", infoLog);
            free(infoLog);
        }
    }
    
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char* infoLog = (char*)malloc(infoLen);
            glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
            LOGE("Program link error: %s", infoLog);
            free(infoLog);
        }
    }
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    LOGI("createShaderProgram: Shader program created!");
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
    
    LOGI("createRectangle: Rectangle created!");
}

void Lobby::init() {
    LOGI("init: Starting...");
    createShaderProgram();
    createRectangle();
    LOGI("init: Lobby initialized!");
}

bool Lobby::update(float dt) {
    return startGame;
}

void Lobby::draw() {
    if (program == 0) {
        LOGE("draw: Program not initialized!");
        return;
    }
    
    glUseProgram(program);
    
    float ortho[16] = {
        2.0f/1280, 0, 0, 0,
        0, -2.0f/720, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1
    };
    
    GLint transformLoc = glGetUniformLocation(program, "uTransform");
    if (transformLoc < 0) {
        LOGE("draw: uTransform location not found!");
        return;
    }
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, ortho);
    
    GLint colorLoc = glGetUniformLocation(program, "uColor");
    if (colorLoc < 0) {
        LOGE("draw: uColor location not found!");
        return;
    }
    glUniform4f(colorLoc, 140.0f/255, 51.0f/255, 217.0f/255, 1.0f);
    
    GLint posLoc = glGetAttribLocation(program, "aPos");
    if (posLoc < 0) {
        LOGE("draw: aPos location not found!");
        return;
    }
    
    glEnableVertexAttribArray(posLoc);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), 0);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    LOGI("draw: Button drawn!");
}

void Lobby::handleTouch(float x, float y) {
    float nx = (x / 1280.0f) * 2 - 1;
    float ny = -(y / 720.0f) * 2 + 1;
    
    LOGI("handleTouch: normalized (%.2f, %.2f)", nx, ny);
    
    if (nx > -0.2f && nx < 0.2f && ny > -0.1f && ny < 0.1f) {
        startGame = true;
        LOGI("handleTouch: Play button pressed!");
    }
}

} // namespace lobby
