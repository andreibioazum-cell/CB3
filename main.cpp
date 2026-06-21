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

// ============================================================
// ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ
// ============================================================

EGLDisplay display = EGL_NO_DISPLAY;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;
ANativeWindow* nativeWindow = nullptr;

lobby::Lobby lobby;
game::Game game;

enum GameState { STATE_LOBBY, STATE_GAME };
GameState currentState = STATE_LOBBY;

// ============================================================
// OPENGL INIT
// ============================================================

bool initOpenGL(ANativeWindow* window) {
    nativeWindow = window;
    
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) return false;
    
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) return false;
    
    EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) return false;
    
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) return false;
    
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE) return false;
    
    if (!eglMakeCurrent(display, surface, surface, context)) return false;
    
    LOGI("OpenGL ES 2.0 initialized!");
    return true;
}

// ============================================================
// JNI ФУНКЦИИ
// ============================================================

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeInit(JNIEnv* env, jobject obj, jobject surface) {
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    
    if (!initOpenGL(window)) {
        LOGI("Failed to init OpenGL!");
        return;
    }
    
    lobby.init();
    game.init();
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeUpdate(JNIEnv* env, jobject obj, jfloat dt) {
    if (currentState == STATE_LOBBY) {
        if (lobby.update(dt)) currentState = STATE_GAME;
    } else {
        game.update(dt);
        if (game.isBackToLobby()) currentState = STATE_LOBBY;
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeDraw(JNIEnv* env, jobject obj) {
    glClearColor(0.1f, 0.05f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (currentState == STATE_LOBBY) {
        lobby.draw();
    } else {
        game.draw();
    }
    
    eglSwapBuffers(display, surface);
}
