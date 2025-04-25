#ifndef FIGUR_H
#define FIGUR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class Figur
{
public:
    Figur();
    ~Figur();

    // Setzt die Modellmatrix, um die Figur (z. B. Positionierung auf dem Brett) zu transformieren
    void setModelMatrix(const glm::mat4 &model);

    // Setzt den lokalen Transform (Position relativ zum Brett)
    void setLocalTransform(const glm::mat4 &local);
    glm::mat4 getLocalTransform() const;

    // Setzt die Farbe der Figur 
    void setColor(const glm::vec3 &color);
    glm::vec3 getColor() const;

    // Rendert die Figur (zuerst Zylinder, dann Kugel)
    // Die finale Modellmatrix wird in main() berechnet als: boardModel * localTransform.
    void render();

private:
    // Shaderprogramm-ID für die Figur (hier wird derselbe Shader genutzt wie beim Board)
    unsigned int shaderID;
    // Modellmatrix zur Transformation der Figur
    glm::mat4 modelMatrix;     // Final (global) Modellmatrix der Figur (Board * local)
    glm::mat4 localTransform;  // Lokaler Transform relativ zum Brett
    glm::vec3 objectColor;     // Farbe der Figur

    // --- Zylinder (Körper) ---
    unsigned int cylinderVAO, cylinderVBO, cylinderEBO;
    unsigned int cylinderIndexCount;

    // --- Kugel (Kopf) ---
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    unsigned int sphereIndexCount;

    // Hilfsfunktionen zum Aufbau der Geometrie
    void setupFigur();
    void setupCylinder();
    void setupSphere();

    // Funktion zum Kompilieren und Linken eines Shaderprogramms
    unsigned int compileShader(const std::string &vertexPath, const std::string &fragmentPath);
};

#endif 