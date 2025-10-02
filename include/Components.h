#pragma once
#include <glm/glm.hpp> // Pour les vecteurs 3D (tu devras installer GLM)

// ============================================
// Transform Component - Position, rotation, scale
// ============================================
struct Transform {
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f}; // Euler angles
    glm::vec3 scale{1.0f, 1.0f, 1.0f};

    Transform() = default;
    Transform(const glm::vec3& pos) : position(pos) {}
};

// ============================================
// Velocity Component - Vitesse de déplacement
// ============================================
struct Velocity {
    glm::vec3 linear{0.0f, 0.0f, 0.0f};
    glm::vec3 angular{0.0f, 0.0f, 0.0f};

    Velocity() = default;
    Velocity(const glm::vec3& vel) : linear(vel) {}
};

// ============================================
// RigidBody Component - Propriétés physiques
// ============================================
struct RigidBody {
    float mass = 1.0f;
    float drag = 0.01f;
    bool useGravity = true;

    RigidBody() = default;
    RigidBody(float m, bool gravity = true) 
        : mass(m), useGravity(gravity) {}
};

// ============================================
// Mesh Component - Référence vers un mesh 3D
// ============================================
struct Mesh {
    uint32_t meshID = 0; // ID du mesh dans le resource manager
    uint32_t materialID = 0; // ID du matériau

    Mesh() = default;
    Mesh(uint32_t mesh, uint32_t mat = 0) 
        : meshID(mesh), materialID(mat) {}
};

// ============================================
// Camera Component - Caméra pour le rendu
// ============================================
struct Camera {
    float fov = 45.0f; // Field of view
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    bool isActive = true;

    Camera() = default;
    Camera(float fieldOfView) : fov(fieldOfView) {}
};

// ============================================
// Tag Component - Simple identifiant textuel
// ============================================
struct Tag {
    std::string name = "Entity";

    Tag() = default;
    Tag(const std::string& n) : name(n) {}
};