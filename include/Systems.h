#pragma once
#include "ECS.h"
#include "Components.h"
#include <iostream>

// ============================================
// PhysicsSystem - Gère le mouvement et la physique
// ============================================
class PhysicsSystem : public System {
public:
    void Update(Coordinator& coordinator, double deltaTime) {
        const float dt = static_cast<float>(deltaTime);
        const glm::vec3 gravity(0.0f, -9.81f, 0.0f);

        for (auto const& entity : m_Entities) {
            auto& transform = coordinator.GetComponent<Transform>(entity);
            auto& velocity = coordinator.GetComponent<Velocity>(entity);
            auto& rigidBody = coordinator.GetComponent<RigidBody>(entity);

            // Appliquer la gravité si activée
            if (rigidBody.useGravity) {
                velocity.linear += gravity * dt;
            }

            // Appliquer le drag (friction de l'air)
            velocity.linear *= (1.0f - rigidBody.drag);

            // Mettre à jour la position
            transform.position += velocity.linear * dt;

            // Mettre à jour la rotation
            transform.rotation += velocity.angular * dt;

            // Debug: afficher la position toutes les 60 frames
            static int frameCount = 0;
            if (++frameCount % 60 == 0) {
                auto& tag = coordinator.GetComponent<Tag>(entity);
                std::cout << tag.name << " - Position: (" 
                          << transform.position.x << ", "
                          << transform.position.y << ", "
                          << transform.position.z << ")" << std::endl;
            }
        }
    }
};

// ============================================
// RenderSystem - Gère le rendu (pour l'instant juste du debug)
// ============================================
class RenderSystem : public System {
public:
    void Render(Coordinator& coordinator) {
        // Pour l'instant, on fait juste un rendu debug
        // Plus tard, on intégrera OpenGL/Vulkan ici
        
        for (auto const& entity : m_Entities) {
        (void)entity; // Évite le warning
        (void)coordinator; // Évite le warning
        // auto& transform = coordinator.GetComponent<Transform>(entity);
        // auto& mesh = coordinator.GetComponent<Mesh>(entity);
            // Ici, on rendrait le mesh à la position du transform
            // Pour l'instant, c'est juste un placeholder
        }
    }
};