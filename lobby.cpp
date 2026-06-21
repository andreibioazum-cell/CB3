#include "lobby.hpp"
#include <android/log.h>
#include <cmath>

#define LOG_TAG "Lobby"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

namespace lobby {

// ============================================================
// ВЕРШИННЫЙ ШЕЙДЕР
// ============================================================
const char* vertexShader = 
    "attribute vec3 aPos;"
    "attribute vec2 aTexCoord;"
    "uniform mat4 uTransform;"
    "varying vec2 vTexCoord;"
    "void main() {"
    "   gl_Position = uTransform * vec4(aPos, 1.0);"
    "   vTexCoord = aTexCoord;"
    "}";

// ============================================================
// ФРАГМЕНТНЫЙ ШЕЙДЕР
// ============================================================
const char* fragmentShader = 
    "precision highp float;"
    "uniform vec4 uColor;"
    "varying vec2 vTexCoord;"
    "void main() {"
    "   gl_FragColor = uColor;"
    "}";

void Lobby::createShaderProgram() {
    // Создаём шейдеры
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
    
    LOGI("Shader program created!");
}

void Lobby::createRectangle() {
    // Квадрат для кнопки Play
    float vertices[] = {
        // x, y, u, v
        -0.2f, -0.1f, 0.0f, 0.0f,
         0.2f, -0.1f, 1.0f, 0.0f,
         0.2f,  0.1f, 1.0f, 1.0f,
        -0.2f,  0.1f, 0.0f, 1.0f
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
    
    // Матрица трансформации (проекция ортогональная)
    float ortho[16] = {
        2.0f/1280, 0, 0, 0,
        0, -2.0f/720, 0, 0,
        0, 0, 1, 0,
        -1, 1, 0, 1
    };
    
    GLint transformLoc = glGetUniformLocation(program, "uTransform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, ortho);
    
    // Рисуем фон (фиолетовый)
    glClearColor(0.1f, 0.05f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рисуем заголовок
    GLint colorLoc = glGetUniformLocation(program, "uColor");
    
    // Кнопка Play
    glUniform4f(colorLoc, 140.0f/255, 51.0f/255, 217.0f/255, 1.0f);
    GLint posLoc = glGetAttribLocation(program, "aPos");
    GLint texLoc = glGetAttribLocation(program, "aTexCoord");
    
    glEnableVertexAttribArray(posLoc);
    glEnableVertexAttribArray(texLoc);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    
    // Рисуем кнопку в центре
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Lobby::handleTouch(float x, float y) {
    // Нормализуем координаты
    float nx = (x / screenWidth) * 2 - 1;
    float ny = -(y / screenHeight) * 2 + 1;
    
    // Проверяем попадание в кнопку Play
    if (nx > -0.2f && nx < 0.2f && ny > -0.1f && ny < 0.1f) {
        startGame = true;
        LOGI("Play button pressed!");
    }
}

} // namespace lobby
