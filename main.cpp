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

// ============================================================
// ПРОСТЕЙШИЙ ШЕЙДЕР
// ============================================================
const char* vertexShader = 
    "attribute vec2 aPos;"
    "void main() {"
    "   gl_Position = vec4(aPos, 0.0, 1.0);"
    "}";

const char* fragmentShader = 
    "precision highp float;"
    "void main() {"
    "   gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"  // ЗЕЛЁНЫЙ!
    "}";

GLuint program = 0;
GLuint vbo = 0;

// ============================================================
// ИНИЦИАЛИЗАЦИЯ OPENGL
// ============================================================
bool initOpenGL(ANativeWindow* window) {
    LOGI("=== initOpenGL START ===");
    
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed!");
        return false;
    }
    LOGI("eglGetDisplay OK");
    
    EGLint major, minor;
    if (!eglInitialize(display, &major, &minor)) {
        LOGE("eglInitialize failed!");
        return false;
    }
    LOGI("eglInitialize OK, version %d.%d", major, minor);
    
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
        LOGE("eglChooseConfig failed!");
        return false;
    }
    LOGI("eglChooseConfig OK, numConfigs: %d", numConfigs);
    
    EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed!");
        return false;
    }
    LOGI("eglCreateContext OK");
    
    surface = eglCreateWindowSurface(display, config, window, nullptr);
    if (surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed!");
        return false;
    }
    LOGI("eglCreateWindowSurface OK");
    
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("eglMakeCurrent failed!");
        return false;
    }
    LOGI("eglMakeCurrent OK");
    
    const char* version = (const char*)glGetString(GL_VERSION);
    LOGI("OpenGL Version: %s", version ? version : "unknown");
    
    LOGI("=== initOpenGL SUCCESS ===");
    return true;
}

// ============================================================
// СОЗДАНИЕ ШЕЙДЕРОВ
// ============================================================
bool createShaders() {
    LOGI("=== createShaders START ===");
    
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShader, nullptr);
    glCompileShader(vs);
    
    GLint compiled;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        LOGE("Vertex shader compile failed!");
        char log[512];
        glGetShaderInfoLog(vs, 512, nullptr, log);
        LOGE("Log: %s", log);
        return false;
    }
    LOGI("Vertex shader compiled");
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShader, nullptr);
    glCompileShader(fs);
    
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        LOGE("Fragment shader compile failed!");
        char log[512];
        glGetShaderInfoLog(fs, 512, nullptr, log);
        LOGE("Log: %s", log);
        return false;
    }
    LOGI("Fragment shader compiled");
    
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        LOGE("Program link failed!");
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        LOGE("Log: %s", log);
        return false;
    }
    LOGI("Program linked");
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    LOGI("=== createShaders SUCCESS ===");
    return true;
}

// ============================================================
// СОЗДАНИЕ КВАДРАТА
// ============================================================
void createQuad() {
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };
    
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    LOGI("Quad created");
}

// ============================================================
// JNI ФУНКЦИИ
// ============================================================
extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeInit(JNIEnv* env, jobject obj, jobject surface) {
    LOGI("=== nativeInit START ===");
    
    ANativeWindow* window = ANativeWindow_fromSurface(env, surface);
    if (window == nullptr) {
        LOGE("ANativeWindow_fromSurface failed!");
        return;
    }
    LOGI("ANativeWindow OK");
    
    if (!initOpenGL(window)) {
        LOGE("initOpenGL FAILED!");
        return;
    }
    
    if (!createShaders()) {
        LOGE("createShaders FAILED!");
        return;
    }
    
    createQuad();
    
    LOGI("=== nativeInit SUCCESS ===");
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeUpdate(JNIEnv* env, jobject obj, jfloat dt) {
    // Ничего не делаем
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeDraw(JNIEnv* env, jobject obj) {
    // Проверяем контекст
    if (display == EGL_NO_DISPLAY || surface == EGL_NO_SURFACE) {
        LOGE("nativeDraw: EGL not initialized!");
        return;
    }
    
    // Делаем контекст текущим
    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOGE("nativeDraw: eglMakeCurrent failed!");
        return;
    }
    
    // Очищаем экран КРАСНЫМ (чтобы видеть, что хоть что-то работает)
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Рисуем зелёный квадрат
    if (program != 0 && vbo != 0) {
        glUseProgram(program);
        
        GLint posLoc = glGetAttribLocation(program, "aPos");
        glEnableVertexAttribArray(posLoc);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        LOGI("nativeDraw: Square drawn!");
    } else {
        LOGE("nativeDraw: program=%d, vbo=%d", program, vbo);
    }
    
    // Меняем буферы
    if (!eglSwapBuffers(display, surface)) {
        LOGE("nativeDraw: eglSwapBuffers failed!");
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_cubicbattle_GameActivity_nativeTouch(JNIEnv* env, jobject obj, jfloat x, jfloat y, jboolean pressed) {
    LOGI("Touch: x=%.1f y=%.1f pressed=%d", x, y, pressed);
}
