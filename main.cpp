#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <string>
#include <vector>
#include <cmath>

#define LOG_TAG "CubicBattle"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ============================================================
// ПРОСТАЯ ИГРА БЕЗ SFML (OpenGL ES)
// ============================================================

struct Point {
    float x, y;
};

class Lobby {
public:
    bool startGame = false;
    
    void update() {}
    void draw() {}
};

class Enemy {
public:
    Point pos = {400, 300};
    float angle = 0;
    int hp = 5;
    bool alive = false;
    
    void reset() { alive = false; }
};

class Game {
public:
    Point player = {640, 360};
    float angle = 0;
    int hp = 5;
    Enemy enemy;
    bool backToLobby = false;
    
    void init() {
        enemy.reset();
    }
    
    void update(float dt) {
        // Простая логика
    }
    
    void draw() {
        // OpenGL рисование
    }
};

Lobby lobby;
Game game;

// ============================================================
// JNI ФУНКЦИИ
// ============================================================

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeInit(JNIEnv* env, jobject obj) {
    LOGI("Game initialized!");
    game.init();
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeUpdate(JNIEnv* env, jobject obj, jfloat dt) {
    game.update(dt);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_cubicbattle_GameActivity_nativeGetHP(JNIEnv* env, jobject obj) {
    return game.hp;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_cubicbattle_GameActivity_nativeGetTitle(JNIEnv* env, jobject obj) {
    return env->NewStringUTF("CUBIC BATTLE 3");
}
