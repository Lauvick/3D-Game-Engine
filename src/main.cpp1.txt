#include "GameEngine.h"
#include "Components.h"
#include "Systems.h"
#include "Shaders.h"
#include "Camera.h"
#include <glad/glad.h>
#include <iostream>

// Variables globales pour la souris
float lastX = 640.0f;
float lastY = 360.0f;
bool firstMouse = true;

// Pointeur vers la caméra (pour les callbacks)
FPSCamera* g_camera = nullptr;

// Callback pour le mouvement de la souris
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    (void)window;
    
    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    float yoffset = lastY - static_cast<float>(ypos); // Inversé car y va de bas en haut

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    if (g_camera) {
        g_camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

// ============================================
// MyGame - Notre jeu avec contrôles caméra
// ============================================
class MyGame : public GameEngine {
public:
    void Init() override {
        std::cout << "=== Initializing MyGame with Camera Controls ===" << std::endl;

        // Initialiser le renderer OpenGL
        GameEngine::Init();

        // Configurer la souris
        glfwSetInputMode(GetRenderer().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(GetRenderer().GetWindow(), mouse_callback);

        // Initialiser la caméra
        m_Camera = FPSCamera(glm::vec3(0.0f, 0.0f, 5.0f));
        g_camera = &m_Camera;

        // Créer les shaders
        m_Shader = new Shader(vertexShaderSource, fragmentShaderSource);

        // Créer un cube coloré
        CreateCube();

        std::cout << "=== Controls ===" << std::endl;
        std::cout << "WASD - Move camera" << std::endl;
        std::cout << "Space/Shift - Up/Down" << std::endl;
        std::cout << "Mouse - Look around" << std::endl;
        std::cout << "ESC - Exit" << std::endl;
        std::cout << "=== MyGame initialized successfully! ===" << std::endl;
    }

    void CreateCube() {
        // Vertices du cube avec couleurs (position + couleur)
        float vertices[] = {
            // Position          // Couleur
            -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // Rouge
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // Vert
             0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  // Bleu
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  // Jaune
            
            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  // Magenta
             0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  // Cyan
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // Blanc
            -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f   // Gris
        };

        unsigned int indices[] = {
            // Face avant
            0, 1, 2,
            2, 3, 0,
            // Face arrière
            4, 5, 6,
            6, 7, 4,
            // Face gauche
            0, 3, 7,
            7, 4, 0,
            // Face droite
            1, 5, 6,
            6, 2, 1,
            // Face haut
            3, 2, 6,
            6, 7, 3,
            // Face bas
            0, 1, 5,
            5, 4, 0
        };

        // Créer les buffers OpenGL
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void ProcessInput(double deltaTime) override {
        auto window = GetRenderer().GetWindow();
        float dt = static_cast<float>(deltaTime);

        // Fermer avec Escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            GetRenderer().SetShouldClose(true);
        }

        // Déplacement WASD
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_Camera.ProcessKeyboard(FORWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_Camera.ProcessKeyboard(BACKWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_Camera.ProcessKeyboard(LEFT, dt);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_Camera.ProcessKeyboard(RIGHT, dt);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_Camera.ProcessKeyboard(UP, dt);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            m_Camera.ProcessKeyboard(DOWN, dt);
    }

    void Update(double deltaTime) override {
        // Faire tourner le cube
        m_Rotation += static_cast<float>(deltaTime) * 50.0f;
        if (m_Rotation > 360.0f) m_Rotation -= 360.0f;
    }

    void Render() override {
        m_Shader->Use();

        // Matrices de transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(m_Rotation), glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = m_Camera.GetViewMatrix();

        glm::mat4 projection = glm::perspective(
            glm::radians(m_Camera.Zoom),
            (float)GetRenderer().GetWidth() / (float)GetRenderer().GetHeight(),
            0.1f,
            100.0f
        );

        // Envoyer les matrices au shader
        m_Shader->SetMat4("model", model);
        m_Shader->SetMat4("view", view);
        m_Shader->SetMat4("projection", projection);

        // Dessiner le cube
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void Cleanup() override {
        std::cout << "=== Cleaning up MyGame ===" << std::endl;
        
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        delete m_Shader;
        
        g_camera = nullptr;
        
        GameEngine::Cleanup();
    }

private:
    Shader* m_Shader = nullptr;
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    float m_Rotation = 0.0f;
    FPSCamera m_Camera;
};

// ============================================
// Point d'entrée du programme
// ============================================
int main() {
    try {
        MyGame game;
        game.Init();
        game.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}