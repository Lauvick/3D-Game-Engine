#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

// ============================================
// Mesh - Structure pour stocker un modèle 3D
// ============================================
struct MeshData {
    std::vector<float> vertices;  // Position + Normale (x,y,z, nx,ny,nz)
    std::vector<unsigned int> indices;
    
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
    
    void SetupMesh() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

        // Normal attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);
    }
    
    void Draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void Cleanup() {
        if (VAO != 0) glDeleteVertexArrays(1, &VAO);
        if (VBO != 0) glDeleteBuffers(1, &VBO);
        if (EBO != 0) glDeleteBuffers(1, &EBO);
    }
};

// ============================================
// OBJLoader - Charge des fichiers .obj
// ============================================
class OBJLoader {
public:
    static bool LoadOBJ(const std::string& filepath, MeshData& mesh) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open OBJ file: " << filepath << std::endl;
            return false;
        }

        std::vector<glm::vec3> temp_vertices;
        std::vector<glm::vec3> temp_normals;
        std::vector<unsigned int> vertex_indices;
        std::vector<unsigned int> normal_indices;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                // Vertex position
                glm::vec3 vertex;
                iss >> vertex.x >> vertex.y >> vertex.z;
                temp_vertices.push_back(vertex);
            }
            else if (prefix == "vn") {
                // Vertex normal
                glm::vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                temp_normals.push_back(normal);
            }
            else if (prefix == "f") {
                // Face (triangle)
                std::string vertex1, vertex2, vertex3;
                iss >> vertex1 >> vertex2 >> vertex3;

                // Parse face format: v/vt/vn ou v//vn ou v
                ParseFace(vertex1, vertex_indices, normal_indices);
                ParseFace(vertex2, vertex_indices, normal_indices);
                ParseFace(vertex3, vertex_indices, normal_indices);
            }
        }

        file.close();

        // Si pas de normales, on les génère
        if (temp_normals.empty()) {
            temp_normals.resize(temp_vertices.size(), glm::vec3(0.0f));
            GenerateNormals(temp_vertices, vertex_indices, temp_normals);
        }

        // Construire le mesh final
        for (size_t i = 0; i < vertex_indices.size(); i++) {
            unsigned int vi = vertex_indices[i];
            unsigned int ni = (i < normal_indices.size()) ? normal_indices[i] : vi;

            if (vi < temp_vertices.size()) {
                glm::vec3 v = temp_vertices[vi];
                glm::vec3 n = (ni < temp_normals.size()) ? temp_normals[ni] : glm::vec3(0.0f, 1.0f, 0.0f);

                mesh.vertices.push_back(v.x);
                mesh.vertices.push_back(v.y);
                mesh.vertices.push_back(v.z);
                mesh.vertices.push_back(n.x);
                mesh.vertices.push_back(n.y);
                mesh.vertices.push_back(n.z);

                mesh.indices.push_back(static_cast<unsigned int>(mesh.indices.size()));
            }
        }

        std::cout << "Loaded OBJ: " << filepath << std::endl;
        std::cout << "  Vertices: " << temp_vertices.size() << std::endl;
        std::cout << "  Triangles: " << mesh.indices.size() / 3 << std::endl;

        mesh.SetupMesh();
        return true;
    }

private:
    static void ParseFace(const std::string& face, 
                          std::vector<unsigned int>& vertex_indices,
                          std::vector<unsigned int>& normal_indices) {
        std::istringstream iss(face);
        std::string vi, ti, ni;

        // Format: v/vt/vn
        if (std::getline(iss, vi, '/')) {
            vertex_indices.push_back(std::stoi(vi) - 1); // OBJ indices start at 1

            if (std::getline(iss, ti, '/')) {
                // Skip texture coordinate
                if (std::getline(iss, ni)) {
                    normal_indices.push_back(std::stoi(ni) - 1);
                }
            }
        }
    }

    static void GenerateNormals(const std::vector<glm::vec3>& vertices,
                                const std::vector<unsigned int>& indices,
                                std::vector<glm::vec3>& normals) {
        // Generate flat normals for each triangle
        for (size_t i = 0; i < indices.size(); i += 3) {
            unsigned int i0 = indices[i];
            unsigned int i1 = indices[i + 1];
            unsigned int i2 = indices[i + 2];

            if (i0 < vertices.size() && i1 < vertices.size() && i2 < vertices.size()) {
                glm::vec3 v0 = vertices[i0];
                glm::vec3 v1 = vertices[i1];
                glm::vec3 v2 = vertices[i2];

                glm::vec3 edge1 = v1 - v0;
                glm::vec3 edge2 = v2 - v0;
                glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

                normals[i0] += normal;
                normals[i1] += normal;
                normals[i2] += normal;
            }
        }

        // Normalize all normals
        for (auto& n : normals) {
            n = glm::normalize(n);
        }
    }
};

// ============================================
// Fonction helper pour créer un modèle simple (sphère)
// ============================================
class MeshGenerator {
public:
    static MeshData CreateSphere(float radius = 1.0f, int sectors = 36, int stacks = 18) {
        MeshData mesh;

        float x, y, z, xy;
        float nx, ny, nz;
        float sectorStep = 2 * glm::pi<float>() / sectors;
        float stackStep = glm::pi<float>() / stacks;
        float sectorAngle, stackAngle;

        for (int i = 0; i <= stacks; ++i) {
            stackAngle = glm::pi<float>() / 2 - i * stackStep;
            xy = radius * cosf(stackAngle);
            z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectors; ++j) {
                sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                nx = x / radius;
                ny = y / radius;
                nz = z / radius;

                mesh.vertices.push_back(x);
                mesh.vertices.push_back(y);
                mesh.vertices.push_back(z);
                mesh.vertices.push_back(nx);
                mesh.vertices.push_back(ny);
                mesh.vertices.push_back(nz);
            }
        }

        // Generate indices
        unsigned int k1, k2;
        for (int i = 0; i < stacks; ++i) {
            k1 = i * (sectors + 1);
            k2 = k1 + sectors + 1;

            for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
                if (i != 0) {
                    mesh.indices.push_back(k1);
                    mesh.indices.push_back(k2);
                    mesh.indices.push_back(k1 + 1);
                }

                if (i != (stacks - 1)) {
                    mesh.indices.push_back(k1 + 1);
                    mesh.indices.push_back(k2);
                    mesh.indices.push_back(k2 + 1);
                }
            }
        }

        mesh.SetupMesh();
        return mesh;
    }
};