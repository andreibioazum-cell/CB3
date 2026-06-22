#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <string>
#include "lobby.hpp"
#include "game.hpp"

#define LOG_TAG "CubicBattle"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EGLDisplay display = EGL_NO_DISPLAY;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;
ANativeWindow* nativeWindow = nullptr;

lobby::Lobby lobbyInstance;
game::Game gameInstance;

enum GameState { STATE_LOBBY, STATE_GAME };
GameState currentState = STATE_LOBBY;

bool initOpenGL(ANativeWindow* window) {
    LOGI("initOpenGL: Starting...");
    
    nativeWindow = window;
    
    // Получаем дисплей
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("initOpenGL: Failed to get display!");
        return false;
    }
    
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        LOGE("initOpenGL: Failed to initialize EGL!");
        return false;
    }
    LOGI("initOpenGL: EGL initialized, version %d.%d", major, minor);
    
    // Конфигурация
    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        LOGE("initOpenGL: Failed to choose config!");
        return false;
    }
    LOGI("initOpenGL: Config chosen, numConfigs: %d", numConfigs);
    
    // Контекст
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        LOGE("initOpenGL: Failed to create context!");
        return false;
    }
    
    // Поверхность
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        LOGE("initOpenGL: Failed to create surface!");
        return false;
    }
    
    // Делаем контекст текущим
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("initOpenGL: Failed to make current!");
        return false;
    }
    
    LOGI("initOpenGL: OpenGL ES 2.0 initialized successfully!");
    return true;
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeInit(JNIEnv* env, jobject obj, jobject surface) {
    LOGI("nativeInit: Called!");
    
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr) {
        LOGE("nativeInit: Failed to get native window!");
        return;
    }
    
    if (!initOpenGL(window)) {
        LOGE("nativeInit: Failed to init OpenGL!");
        return;
    }
    
    // Проверяем OpenGL
    const char* version = (const char*)glGetString(GL_VERSION);
    LOGI("nativeInit: OpenGL Version: %s", version ? version : "unknown");
    
    // Инициализируем лобби и игру
    lobbyInstance.init();
    gameInstance.init();
    
    LOGI("nativeInit: Complete!");
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeUpdate(JNIEnv* env, jobject obj, jfloat dt) {
    if (currentState == STATE_LOBBY) {
        if (lobbyInstance.update(dt)) {
            LOGI("nativeUpdate: Switching to GAME state!");
            currentState = STATE_GAME;
        }
    } else {
        gameInstance.update(dt);
        if (gameInstance.isBackToLobby()) {
            LOGI("nativeUpdate: Switching to LOBBY state!");
            currentState = STATE_LOBBY;
            gameInstance.reset();
        }
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeDraw(JNIEnv* env, jobject obj) {
    // Проверяем контекст
    if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE || context == EGL_NO_CONTEXT) {
        LOGE("nativeDraw: EGL not initialized!");
        return;
    }
    
    // Делаем контекст текущим (на всякий случай)
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("nativeDraw: Failed to make current!");
        return;
    }
    
    // Очищаем экран КРАСНЫМ для отладки (чтобы видеть, что рендеринг работает)
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  // Красный - тестовый цвет
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рисуем
    if (currentState == STATE_LOBBY) {
        lobbyInstance.draw();
    } else {
        gameInstance.draw();
    }
    
    // Меняем буферы
    if (!eglSwapBuffers(display, surface)) {
        LOGE("nativeDraw: Failed to swap buffers!");
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeTouch(JNIEnv* env, jobject obj, jfloat x, jfloat y, jboolean pressed) {
    LOGI("nativeTouch: x=%.1f, y=%.1f, pressed=%d", x, y, pressed);
    
    if (currentState == STATE_LOBBY) {
        lobbyInstance.handleTouch(x, y);
    } else {
        gameInstance.handleTouch(x, y, pressed);
    }
}

// ============================================================
// ОТЛАДОЧНЫЕ ФУНКЦИИ (можно вызвать из Java для проверки)
// ============================================================

extern "C" JNIEXPORT jstring JNICALL
Java_com_cubicbattle_GameActivity_nativeGetGLVersion(JNIEnv* env, jobject obj) {
    const char* version = (const char*)glGetString(GL_VERSION);
    return env->NewStringUTF(version ? version : "No OpenGL context");
}
