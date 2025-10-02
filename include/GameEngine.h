#pragma once
#include <chrono>
#include <iostream>
#include <thread>
#include "ECS.h"
#include "Renderer.h"

// ============================================
// GameEngine - Gère la boucle principale du jeu
// ============================================
class GameEngine {
public:
    GameEngine() : m_IsRunning(false), m_TargetFPS(60) {
        m_Coordinator.Init();
    }

    virtual ~GameEngine() = default;

    // Initialisation du moteur (à override dans les classes dérivées)
    virtual void Init() {
        std::cout << "GameEngine initialized!" << std::endl;
        
        // Initialiser le renderer
        if (!m_Renderer.Init()) {
            throw std::runtime_error("Failed to initialize renderer");
        }
    }

    // Méthode principale pour lancer le jeu
    void Run() {
        m_IsRunning = true;
        
        // Variables pour le delta time
        auto lastTime = std::chrono::high_resolution_clock::now();
        const double targetFrameTime = 1.0 / m_TargetFPS;
        
        std::cout << "Game loop started (Target FPS: " << m_TargetFPS << ")" << std::endl;

        // Boucle de jeu principale
        while (m_IsRunning && !m_Renderer.ShouldClose()) {
            // Calculer le delta time
            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = currentTime - lastTime;
            double deltaTime = elapsed.count();
            lastTime = currentTime;

            // Gérer les événements (clavier, souris, fenêtre)
            m_Renderer.PollEvents();

            // Mettre à jour le jeu
            ProcessInput(deltaTime);
            Update(deltaTime);
            
            // Rendu
            m_Renderer.Clear(0.1f, 0.1f, 0.15f);
            Render();
            m_Renderer.SwapBuffers();

            // Limiter le framerate (optionnel)
            if (deltaTime < targetFrameTime) {
                std::chrono::duration<double> sleepDuration(targetFrameTime - deltaTime);
                std::this_thread::sleep_for(sleepDuration);
            }

            // Afficher les FPS toutes les secondes
            static double fpsTimer = 0.0;
            static int frameCount = 0;
            fpsTimer += deltaTime;
            frameCount++;

            if (fpsTimer >= 1.0) {
                std::cout << "FPS: " << frameCount << std::endl;
                fpsTimer = 0.0;
                frameCount = 0;
            }
        }

        Cleanup();
    }

    void Stop() {
        m_IsRunning = false;
    }

    // Accès au coordinateur ECS
    Coordinator& GetCoordinator() {
        return m_Coordinator;
    }

    // Accès au renderer
    Renderer& GetRenderer() {
        return m_Renderer;
    }

protected:
    // Méthodes à override dans les classes dérivées
    virtual void ProcessInput(double deltaTime) { (void)deltaTime; }
    virtual void Update(double deltaTime) { (void)deltaTime; }
    virtual void Render() {}
    virtual void Cleanup() {
        std::cout << "GameEngine cleanup" << std::endl;
        m_Renderer.Cleanup();
    }

    Coordinator m_Coordinator;
    Renderer m_Renderer;
    bool m_IsRunning;
    int m_TargetFPS;
};