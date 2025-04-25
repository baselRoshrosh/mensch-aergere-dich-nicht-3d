#ifndef BOARD_H
#define BOARD_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Board
{
private:
    unsigned int VAO, VBO, EBO, texture;
    unsigned int shaderID; 
    glm::mat4 modelMatrix;  // Speichert Transformationen (Rotation)
    void setupBoard();  // Spielfeld-Setup
    void loadTexture(const char* path);  // Textur laden
    unsigned int compileShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

public:
    Board();
    ~Board();

    void setWindowSize(int width, int height);  // Viewport anpassen
    void initGL();  // OpenGL Initialisierung
    void uninitGL(); // OpenGL Ressourcen freigeben

    void render();  // Spielfeld rendern
    

    void rotX(float angle);
    void rotY(float angle);

    void keyPressed(int key);
    glm::mat4 getModelMatrix() const;
};

#endif