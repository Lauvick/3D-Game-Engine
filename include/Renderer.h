#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

// ============================================
// Renderer - Gère OpenGL et la fenêtre
// ============================================
class Renderer {
public:
    Renderer(int width = 1280, int height = 720, const char* title = "GameEngine")
        : m_Width(width), m_Height(height), m_Window(nullptr) {}

    ~Renderer() {
        Cleanup();
    }

    bool Init() {
        // Initialiser GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        // Configuration de GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Créer la fenêtre
        m_Window = glfwCreateWindow(m_Width, m_Height, "GameEngine - OpenGL", nullptr, nullptr);
        if (!m_Window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_Window);
        glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

        // Charger GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        // Configuration OpenGL
        glViewport(0, 0, m_Width, m_Height);
        glEnable(GL_DEPTH_TEST);

        std::cout << "OpenGL Renderer initialized!" << std::endl;
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

        return true;
    }

    void Clear(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 1.0f) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SwapBuffers() {
        glfwSwapBuffers(m_Window);
    }

    void PollEvents() {
        glfwPollEvents();
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(m_Window);
    }

    void SetShouldClose(bool value) {
        glfwSetWindowShouldClose(m_Window, value);
    }

    GLFWwindow* GetWindow() const {
        return m_Window;
    }

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    void Cleanup() {
        if (m_Window) {
            glfwDestroyWindow(m_Window);
            m_Window = nullptr;
        }
        glfwTerminate();
    }

private:
    int m_Width;
    int m_Height;
    GLFWwindow* m_Window;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        (void)window;
        glViewport(0, 0, width, height);
    }
};

// ============================================
// Shader - Gère les shaders OpenGL
// ============================================
class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexSource, const char* fragmentSource) {
        // Compiler le vertex shader
        unsigned int vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
        
        // Compiler le fragment shader
        unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

        // Créer le programme shader
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);

        // Vérifier les erreurs de linking
        CheckCompileErrors(ID, "PROGRAM");

        // Supprimer les shaders (ils sont liés au programme maintenant)
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    void Use() {
        glUseProgram(ID);
    }

    void SetMat4(const std::string& name, const glm::mat4& mat) {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void SetVec3(const std::string& name, const glm::vec3& value) {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
    }

private:
    unsigned int CompileShader(unsigned int type, const char* source) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        CheckCompileErrors(shader, type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        return shader;
    }

    void CheckCompileErrors(unsigned int shader, const std::string& type) {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "Shader compilation error (" << type << "):\n" << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "Shader linking error:\n" << infoLog << std::endl;
            }
        }
    }
};