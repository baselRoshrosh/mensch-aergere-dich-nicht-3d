#include "Figur.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include "ShaderUtils.h"

// GLM für Transformationen
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



// Implementation der Figur-Klasse
Figur::Figur()
    : shaderID(0),
      modelMatrix(glm::mat4(1.0f)),
      cylinderVAO(0), cylinderVBO(0), cylinderEBO(0), cylinderIndexCount(0),
      sphereVAO(0), sphereVBO(0), sphereEBO(0), sphereIndexCount(0)
{
    const std::string vertexShaderPath   = "shader/shader.vert";
    const std::string fragmentShaderPath = "shader/shader.frag";    

    shaderID = compileShader(vertexShaderPath, fragmentShaderPath);
    if (!shaderID)
    {
        std::cerr << "Fehler: Shader für Figur konnte nicht geladen werden!" << std::endl;
    }

    // Aufbau der Geometrie: Zylinder und Kugel
    setupFigur();
}

Figur::~Figur()
{
    // Ressourcen des Zylinders freigeben
    glDeleteVertexArrays(1, &cylinderVAO);
    glDeleteBuffers(1, &cylinderVBO);
    glDeleteBuffers(1, &cylinderEBO);

    // Ressourcen der Kugel freigeben
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);

    // Shader löschen
    glDeleteProgram(shaderID);
}

void Figur::setModelMatrix(const glm::mat4 &model)
{
    modelMatrix = model;
}

void Figur::setLocalTransform(const glm::mat4 &local) {
    localTransform = local;
}

glm::mat4 Figur::getLocalTransform() const {
    return localTransform;
}

void Figur::setColor(const glm::vec3 &color) {
    objectColor = color;
}

glm::vec3 Figur::getColor() const {
    return objectColor;
}


void Figur::render()
{
    if (!shaderID)
        return;

    glUseProgram(shaderID);

    int useTextureLoc = glGetUniformLocation(shaderID, "useTexture");
    glUniform1i(useTextureLoc, GL_FALSE);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 3.0f, 10.0f), // Kamera-Position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Blickpunkt
        glm::vec3(0.0f, 1.0f, 0.0f)   // Up-Vektor
    );

    // die Uniforms "model", "view" und "projection"
    int modelLoc = glGetUniformLocation(shaderID, "model");
    int viewLoc = glGetUniformLocation(shaderID, "view");
    int projLoc = glGetUniformLocation(shaderID, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Uniform für die Objektfarbe.
    int colorLoc = glGetUniformLocation(shaderID, "objectColor");
    if (colorLoc != -1) {
        glUniform3fv(colorLoc, 1, glm::value_ptr(objectColor));
    }

    // Zeichnet Zylinder (Körper)
    glBindVertexArray(cylinderVAO);
    glDrawElements(GL_TRIANGLES, cylinderIndexCount, GL_UNSIGNED_INT, 0);

    // Zeichne Kugel (Kopf)
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}


// Aufbau der Geometrie
void Figur::setupFigur()
{
    setupCylinder();
    setupSphere();
}


// Zylinder (Körper)
void Figur::setupCylinder()
{
    // Parameter für den Zylinder
    const int segments = 36;
    const float radius = 0.3f;
    const float height = 1.0f;
    const float taperFactor = 0.5f; // Der obere Kreis hat 50% des unteren Radius
    const float topRadius = radius * taperFactor;

    std::vector<float> vertices; // Jeder Vertex: Position (3 floats) + Dummy-Texturkoordinate (2 floats)
    std::vector<unsigned int> indices;

    // Erzeuge (segments+1)*2 Vertices (doppelt, um den Kreis zu schließen)
    for (int i = 0; i <= segments; ++i)
    {
        float theta = 2.0f * M_PI * i / segments;
        float x = radius * cos(theta);
        float y = radius * sin(theta);
        float x_top = topRadius * cos(theta);
        float y_top = topRadius * sin(theta);

        // Unterer Rand (z = 0)
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f);
        // Dummy-Texturkoordinate
        vertices.push_back((float)i / segments);
        vertices.push_back(0.0f);

        // Oberer Rand (z = height)
        vertices.push_back(x_top);
        vertices.push_back(y_top);
        vertices.push_back(height);
        // Dummy-Texturkoordinate
        vertices.push_back((float)i / segments);
        vertices.push_back(1.0f);
    }

    // Indizes für die Seitenfläche: Für jedes Segment zwei Dreiecke
    for (int i = 0; i < segments; ++i)
    {
        int indexBottom0 = 2 * i;
        int indexTop0 = indexBottom0 + 1;
        int indexBottom1 = 2 * (i + 1);
        int indexTop1 = indexBottom1 + 1;

        // Erstes Dreieck
        indices.push_back(indexBottom0);
        indices.push_back(indexTop0);
        indices.push_back(indexTop1);

        // Zweites Dreieck
        indices.push_back(indexBottom0);
        indices.push_back(indexTop1);
        indices.push_back(indexBottom1);
    }
    cylinderIndexCount = static_cast<unsigned int>(indices.size());

    // Erstelle VAO, VBO und EBO für den Zylinder
    glGenVertexArrays(1, &cylinderVAO);
    glGenBuffers(1, &cylinderVBO);
    glGenBuffers(1, &cylinderEBO);

    glBindVertexArray(cylinderVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribut 0: Position (3 floats), Attribut 1: Dummy-Texturkoordinate (2 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


// Kugel (Kopf)
void Figur::setupSphere()
{
    // Parameter für die Kugel
    const int latSegments = 18;
    const int longSegments = 36;
    const float sphereRadius = 0.35f;
    // Die Kugel soll oben auf dem Zylinder sitzen.
    const float cylinderHeight = 1.0f;
    // Versetze die Kugel so, dass ihr Zentrum bei (0,0, cylinderHeight + sphereRadius - Offset) liegt.
    const float sphereOffsetZ = cylinderHeight + sphereRadius - 0.3f;

    std::vector<float> vertices; // Jeder Vertex: Position (3) + Dummy-Texturkoordinate (2)
    std::vector<unsigned int> indices;

    // Erzeuge die Vertices mittels sphärischer Koordinaten
    for (int i = 0; i <= latSegments; ++i)
    {
        float phi = M_PI * i / latSegments; // Winkel von 0 bis PI
        for (int j = 0; j <= longSegments; ++j)
        {
            float theta = 2.0f * M_PI * j / longSegments; // Winkel von 0 bis 2PI

            float x = sphereRadius * sin(phi) * cos(theta);
            float y = sphereRadius * sin(phi) * sin(theta);
            float z = sphereRadius * cos(phi);

            // Versetze die Kugel nach oben
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z + sphereOffsetZ);
            // Dummy-Texturkoordinate
            vertices.push_back((float)j / longSegments);
            vertices.push_back((float)i / latSegments);
        }
    }

    // Indizes: Verbinde die Vertices zu Dreiecken
    for (int i = 0; i < latSegments; ++i)
    {
        for (int j = 0; j < longSegments; ++j)
        {
            int first = i * (longSegments + 1) + j;
            int second = first + longSegments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    sphereIndexCount = static_cast<unsigned int>(indices.size());

    // Erstelle VAO, VBO und EBO für die Kugel
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribut 0: Position, Attribut 1: Dummy-Texturkoordinate
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// Shader-Kompilierung (ähnlich wie in Board.cpp)
unsigned int Figur::compileShader(const std::string &vertexPath, const std::string &fragmentPath)
{
    // Vertex-Shader laden
    std::string vertexCode = readShaderFile(vertexPath);
    if (vertexCode.empty())
    {
        std::cerr << "Fehler: Vertex-Shader konnte nicht geladen werden!" << std::endl;
        return 0;
    }
    const char *vertexSource = vertexCode.c_str();
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Fehler beim Kompilieren des Vertex Shaders:\n" << infoLog << std::endl;
        return 0;
    }

    // Fragment-Shader laden
    std::string fragmentCode = readShaderFile(fragmentPath);
    if (fragmentCode.empty())
    {
        std::cerr << "Fehler: Fragment-Shader konnte nicht geladen werden!" << std::endl;
        return 0;
    }
    const char *fragmentSource = fragmentCode.c_str();
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fehler beim Kompilieren des Fragment Shaders:\n" << infoLog << std::endl;
        return 0;
    }

    // Erstelle und linke das Shaderprogramm
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Fehler beim Linken des Shader-Programms:\n" << infoLog << std::endl;
        return 0;
    }

    // Lösche die Einzelshaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}