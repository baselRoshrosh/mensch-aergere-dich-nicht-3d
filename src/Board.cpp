#define STB_IMAGE_IMPLEMENTATION
#include "Board.h"
#include "stb/stb_image.h"
#include "ShaderUtils.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <fstream>
#include <sstream>



// Shader-Kompilierungsfunktion
unsigned int Board::compileShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
    // Vertex Shader laden und kompilieren
    std::string vertexCode = readShaderFile(vertexShaderPath);
    if (vertexCode.empty()) {
        std::cerr << "Fehler: Vertex-Shader konnte nicht geladen werden!" << std::endl;
        return 0;
    }
    const char* vertexSource = vertexCode.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Fehler beim Kompilieren des Vertex Shaders:\n" << infoLog << std::endl;
        return 0;
    }

    // Fragment Shader laden und kompilieren
    std::string fragmentCode = readShaderFile(fragmentShaderPath);
    if (fragmentCode.empty()) {
        std::cerr << "Fehler: Fragment-Shader konnte nicht geladen werden!" << std::endl;
        return 0;
    }
    const char* fragmentSource = fragmentCode.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fehler beim Kompilieren des Fragment Shaders:\n" << infoLog << std::endl;
        return 0;
    }
  

    // Shader-Programm erstellen und linken
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Fehler beim Linken des Shader-Programms:\n" << infoLog << std::endl;
        return 0;
    }

    // Shader löschen, da sie im Programm sind
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void checkOpenGLError(const std::string& functionName)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Fehler (" << functionName << "): " << err << std::endl;
    }
}


Board::Board() 
    : shaderID(0) // Initialisiere shaderID mit 0
{
    const std::string vertexShaderPath   = "shader/shader.vert";
    const std::string fragmentShaderPath = "shader/shader.frag";

    shaderID = compileShader(vertexShaderPath, fragmentShaderPath);
    if (!shaderID) {
        std::cerr << "Fehler: Shader konnte nicht geladen werden!" << std::endl;
    }

    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 1.0f));
    setupBoard();
}

Board::~Board()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
}

void Board::setupBoard()
{
    float textureAspectRatio = 1024.0f / 768.0f; // 1.333

    // 3D-Quader für das Brett (Position & Textur-Koordinaten)
    float vertices[] = {
    //  Position          // Texture
    -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, // Links unten
     0.5f, -0.5f, 0.0f,   1.0f, 1.0f, // Rechts unten
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, // Rechts oben
    -0.5f,  0.5f, 0.0f,   0.0f, 0.0f  // Links oben

    };

    unsigned int indices[] = {  
        // Vorderseite
        0, 1, 2, 2, 3, 0,
       
    };

    glGenVertexArrays(1, &VAO);
    checkOpenGLError("glGenVertexArrays");

    glGenBuffers(1, &VBO);
    checkOpenGLError("glGenBuffers (VBO)");

    std::cout << "VAO ID: " << VAO << ", VBO ID: " << VBO << ", EBO ID: " << EBO << std::endl;

    glGenBuffers(1, &EBO);
    checkOpenGLError("glGenBuffers (EBO)");

    glBindVertexArray(VAO);
    checkOpenGLError("glBindVertexArray");

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    checkOpenGLError("glBufferData (VBO)");



    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    checkOpenGLError("glBufferData (EBO)");

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    checkOpenGLError("glVertexAttribPointer (Position)");
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    checkOpenGLError("glVertexAttribPointer (Texture)");

    glBindVertexArray(0);

    loadTexture("textures/board.jpg");

}

void Board::loadTexture(const char* path)
{
    glGenTextures(1, &texture);
    checkOpenGLError("glGenTextures");
    glBindTexture(GL_TEXTURE_2D, texture);
    checkOpenGLError("glBindTexture");
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkOpenGLError("glTexParameteri");

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);  // RGBA statt RGB
    checkOpenGLError("stbi_load");
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        checkOpenGLError("glTexImage2D");

        glGenerateMipmap(GL_TEXTURE_2D);
        checkOpenGLError("glGenerateMipmap");
        std::cout << "Textur geladen: " << path << " (" << width << "x" << height << " - " << nrChannels << " Kanäle)" << std::endl;
    }
    else
    {
        std::cout << "Fehler: Textur konnte nicht geladen werden!" << std::endl;
    }
    stbi_image_free(data);

    
}

void Board::render()
{
   if (!shaderID) {
        std::cerr << "Fehler: Shader wurde nicht geladen!" << std::endl;
        return;
    }

    glUseProgram(shaderID);  
    checkOpenGLError("glUseProgram");

    int useTextureLoc = glGetUniformLocation(shaderID, "useTexture");
    glUniform1i(useTextureLoc, GL_TRUE);

    // Matrizen für 3D-Transformation
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 3.0f, 10.0f), // Kamera-Position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Blickpunkt
        glm::vec3(0.0f, 1.0f, 0.0f)   // Up-Vektor
    );
    
    unsigned int modelLoc = glGetUniformLocation(shaderID, "model");
    if (modelLoc == -1) {
    std::cerr << "Fehler: Uniform 'model' wurde nicht gefunden!" << std::endl;
    }
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    checkOpenGLError("glUniformMatrix4fv");

    unsigned int viewLoc = glGetUniformLocation(shaderID, "view");
    if (viewLoc == -1) {
    std::cerr << "Fehler: Uniform 'view' wurde nicht gefunden!" << std::endl;
    }
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    unsigned int projectionLoc = glGetUniformLocation(shaderID, "projection");
    if (projectionLoc == -1) {
    std::cerr << "Fehler: Uniform 'projection' wurde nicht gefunden!" << std::endl;
    }
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

   
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    checkOpenGLError("glBindTexture");
    glUniform1i(glGetUniformLocation(shaderID, "texture1"), 0);

    glBindVertexArray(VAO);
    checkOpenGLError("glBindVertexArray");

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    checkOpenGLError("glDrawElements");
   
    glBindVertexArray(0);
}

void Board::setWindowSize(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Board::initGL()
{
    glEnable(GL_DEPTH_TEST);
    std::cout << "OpenGL für Board initialisiert." << std::endl;
}

void Board::uninitGL()
{
    glDeleteTextures(1, &texture);
    std::cout << "OpenGL Ressourcen für Board freigegeben." << std::endl;
}

void Board::rotX(float angle)
{
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
}

void Board::rotY(float angle)
{
    modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Board::keyPressed(int key)
{
    std::cout << "Taste gedrückt: " << key << std::endl;
}

glm::mat4 Board::getModelMatrix() const {
    return modelMatrix;
}