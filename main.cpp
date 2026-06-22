#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#define LOG_TAG "CubicBattle"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

EGLDisplay display = EGL_NO_DISPLAY;
EGLSurface surface = EGL_NO_SURFACE;
EGLContext context = EGL_NO_CONTEXT;

bool initOpenGL(ANativeWindow* window) {
    LOGI("=== initOpenGL START ===");
    
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }
    LOGI("eglGetDisplay OK");
    
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        LOGE("eglInitialize failed");
        return false;
    }
    LOGI("eglInitialize OK: %d.%d", major, minor);
    
    // ПРОСТЕЙШАЯ конфигурация
    EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig failed");
        return false;
    }
    LOGI("eglChooseConfig OK: %d configs", numConfigs);
    
    // Контекст ES 2.0 (проще, чем 3.0)
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }
    LOGI("eglCreateContext OK");
    
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }
    LOGI("eglCreateWindowSurface OK");
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }
    LOGI("eglMakeCurrent OK");
    
    const char* version = (const char*)glGetString(GL_VERSION);
    LOGI("GL_VERSION: %s", version ? version : "null");
    
    LOGI("=== initOpenGL SUCCESS ===");
    return true;
}

// ============================================================
// JNI ФУНКЦИИ
// ============================================================
extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeInit(JNIEnv* env, jobject obj, jobject surface) {
    LOGI("=== nativeInit ===");
    
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr) {
        LOGE("ANativeWindow_fromSurface failed");
        return;
    }
    LOGI("ANativeWindow OK");
    
    if (!initOpenGL(window)) {
        LOGE("initOpenGL FAILED");
        return;
    }
    
    LOGI("=== nativeInit DONE ===");
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeUpdate(JNIEnv* env, jobject obj, jfloat dt) {
    // Пусто
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeDraw(JNIEnv* env, jobject obj) {
    if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
        LOGE("draw: EGL not ready");
        return;
    }
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("draw: eglMakeCurrent failed");
        return;
    }
    
    // Заливаем экран КРАСНЫМ
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Проверяем ошибки
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("glError: 0x%x", error);
    }
    
    if (!eglSwapBuffers(display, surface)) {
        LOGE("eglSwapBuffers failed");
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeTouch(JNIEnv* env, jobject obj, jfloat x, jfloat y, jboolean pressed) {
    LOGI("Touch: %.0f, %.0f, %d", x, y, pressed);
}
