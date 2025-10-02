# GameEngine - Moteur de jeu 3D avec ECS

Un moteur de jeu 3D construit en C++ moderne avec une architecture Entity Component System (ECS).

## 🏗️ Structure du projet

```
GameEngine/
├── include/
│   ├── ECS.h              # Entity Component System
│   ├── GameEngine.h       # Boucle de jeu principale
│   ├── Components.h       # Définitions des components
│   └── Systems.h          # Systèmes (Physics, Render, etc.)
├── src/
│   └── main.cpp           # Point d'entrée
├── CMakeLists.txt         # Configuration CMake
└── README.md              # Ce fichier
```

## 📋 Prérequis

- **Compilateur C++17** ou plus récent (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake 3.15+**
- **GLM** (OpenGL Mathematics) - Bibliothèque de mathématiques

### Installation de GLM

**Ubuntu/Debian:**
```bash
sudo apt-get install libglm-dev
```

**macOS (Homebrew):**
```bash
brew install glm
```

**Windows (vcpkg):**
```bash
vcpkg install glm
```

## 🚀 Compilation et exécution

### Méthode 1: Avec CMake (Recommandé)

```bash
# Créer le dossier de build
mkdir build && cd build

# Générer les fichiers de build
cmake ..

# Compiler
cmake --build .

# Exécuter
./GameEngine
```

### Méthode 2: Compilation manuelle

```bash
# Avec GCC/Clang
g++ -std=c++17 -I./include src/main.cpp -o GameEngine

# Avec MSVC
cl /EHsc /std:c++17 /I.\include src\main.cpp
```

## 🎮 Ce que fait le code actuellement

Le programme crée 4 entités de test:

1. **Ball** - Une balle qui tombe avec la gravité
2. **Floating Cube** - Un cube qui flotte sans gravité
3. **Visual Cube** - Un cube avec un mesh (pour le rendu futur)
4. **Main Camera** - Une caméra pour la vue

Le moteur tourne pendant 5 secondes et affiche:
- Les FPS en temps réel
- La position des objets physiques toutes les secondes

## 🧩 Comprendre l'architecture ECS

### Entity (Entité)
- Juste un ID unique (uint32_t)
- Ne contient pas de données
- C'est un "conteneur" pour des components

### Component (Composant)
- Données pures (struct)
- Exemples: `Transform`, `Velocity`, `RigidBody`
- Pas de logique

### System (Système)
- Contient la logique
- Opère sur des entités qui ont certains components
- Exemples: `PhysicsSystem`, `RenderSystem`

### Coordinator
- Interface principale pour tout gérer
- Crée/détruit les entités
- Ajoute/retire des components
- Enregistre les systèmes

## 📚 Concepts C++ utilisés

### C++ Moderne
- ✅ `std::unique_ptr` / `std::shared_ptr` (Smart pointers)
- ✅ Templates avancés
- ✅ `std::bitset` pour les signatures
- ✅ `std::unordered_map` pour les lookups rapides
- ✅ Move semantics
- ✅ RAII (Resource Acquisition Is Initialization)
- ✅ `constexpr` et types forts

### Design Patterns
- ✅ **Entity Component System** (architecture data-oriented)
- ✅ **Manager Pattern** (EntityManager, ComponentManager)
- ✅ **Template Method** (GameEngine virtuel)
- ✅ **Type-safe polymorphism** (IComponentArray)

## 🎓 Comment étendre le moteur

### Ajouter un nouveau Component

```cpp
// Dans Components.h
struct Health {
    float current = 100.0f;
    float max = 100.0f;
};

// Dans MyGame::Init()
m_Coordinator.RegisterComponent<Health>();

// Utilisation
Entity player = m_Coordinator.CreateEntity();
m_Coordinator.AddComponent(player, Health{100.0f, 100.0f});
```

### Ajouter un nouveau System

```cpp
// Dans Systems.h
class HealthSystem : public System {
public:
    void Update(Coordinator& coordinator, double deltaTime) {
        for (auto const& entity : m_Entities) {
            auto& health = coordinator.GetComponent<Health>(entity);
            
            // Ta logique ici
            if (health.current <= 0) {
                // Détruire l'entité
            }
        }
    }
};

// Dans MyGame::Init()
m_HealthSystem = m_Coordinator.RegisterSystem<HealthSystem>();
Signature signature;
signature.set(m_Coordinator.GetComponentType<Health>());
m_Coordinator.SetSystemSignature<HealthSystem>(signature);
```

## 🔄 Prochaines étapes (Phase 2)

### Ce qu'on va ajouter ensuite:

1. **Intégration OpenGL**
   - Créer une fenêtre (GLFW)
   - Rendu basique de triangles
   - Shaders (vertex + fragment)

2. **Système de caméra fonctionnel**
   - Matrices de vue et projection
   - Contrôles caméra (FPS style)

3. **Chargement de modèles 3D**
   - Parser de fichiers OBJ
   - Vertex buffers (VBO/VAO)

4. **Gestion des inputs**
   - Clavier et souris
   - Système d'événements

## 💡 Questions fréquentes

### Pourquoi ECS au lieu de POO classique?

**Performance**: 
- Cache-friendly (données contiguës en mémoire)
- Pas de virtual function calls
- Parallélisation facile

**Flexibilité**:
- Composition over inheritance
- Facile d'ajouter/retirer des comportements

### C'est quoi GLM?

GLM = OpenGL Mathematics. C'est une bibliothèque qui fournit:
- `glm::vec3` - Vecteurs 3D
- `glm::mat4` - Matrices 4x4
- Fonctions mathématiques pour les graphiques

### Pourquoi utiliser CMake?

CMake permet de:
- Build cross-platform (Windows, Linux, macOS)
- Gérer les dépendances facilement
- Configurations Debug/Release automatiques

## 🐛 Débuggage

### Erreur: "glm/glm.hpp not found"

Installer GLM (voir section Prérequis)

### Erreur de compilation C++17

Vérifier la version de votre compilateur:
```bash
g++ --version  # Doit être >= 9.0
```

### Le programme ne compile pas

Vérifier que tu vous bien:
1. Tous les fichiers dans les bons dossiers
2. GLM installé
3. CMake 3.15+

## 📖 Ressources pour apprendre

- [ECS FAQ](https://github.com/SanderMertens/ecs-faq)
- [GLM Documentation](https://glm.g-truc.net/)
- [LearnOpenGL](https://learnopengl.com/) - Pour la phase 2
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)

## 📝 Notes importantes

- Le code est **commenté en français** pour faciliter la compréhension
- Chaque classe a un rôle bien défini (Single Responsibility)
- Le code est **type-safe** et utilise les bonnes pratiques C++
- Pas de raw pointers, que des smart pointers
- Gestion d'erreurs avec exceptions

## 🏆 Points forts

✅ Architecture moderne et scalable
✅ Code propre et bien organisé
✅ Utilisation de C++17/20
✅ Design patterns appropriés
✅ Performance-oriented (cache-friendly)
✅ Extensible et maintenable

---

**Auteur**: Lauvick NGOMA  
**Licence**: MIT  
**Version**: 1.0.0 - Phase 1🎯 Fonctionnalités actuelles

### Phase 1 (Complétée) ✅
- ✅ Architecture ECS complète et performante
- ✅ Boucle de jeu avec delta time
- ✅ Système de physique basique (gravité, vélocité)
- ✅ Gestion d'entités et components
- ✅ Système de rendu (structure seulement)

##