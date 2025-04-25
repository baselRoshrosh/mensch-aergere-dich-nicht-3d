#include "glad/glad.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"
#include "Board.h"  
#include "Figur.h"
#include "ShaderUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Board* g_pcBoard = nullptr; // Neues Board-Objekt für das Spielfeld
std::vector<Figur*> g_figuren; // Container für 16 Figuren

// Globale Variablen für den Hintergrund
unsigned int quadVAO, quadVBO;
unsigned int bgTexture;
unsigned int bgShaderProgram;

void errorCallback(int iError, const char* pcDescription);
void resizeCallback(GLFWwindow* pWindow, int width, int height);
void keyboardCallback(GLFWwindow* pWindow, int iKey, int iScancode, int iAction, int iMods);

// Hilfsfunktion: Shader aus Dateien kompilieren
unsigned int compileShader(const std::string &vertexPath, const std::string &fragmentPath)
{
  std::string vertexCode = readShaderFile(vertexPath);
  std::string fragmentCode = readShaderFile(fragmentPath);
  if(vertexCode.empty() || fragmentCode.empty()){
    return 0;
  }
  const char* vertexSource = vertexCode.c_str();
  const char* fragmentSource = fragmentCode.c_str();

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
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  return shaderProgram;
}

// Hilfsfunktion: Textur laden (nutzt stb_image)
unsigned int loadTexture(const char* path)
{
  // Bild vertikal umdrehen, damit es richtig ausgerichtet ist
  stbi_set_flip_vertically_on_load(true);
  unsigned int textureID;
  glGenTextures(1, &textureID);
  
  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data)
  {
    GLenum format;
    if(nrChannels == 1)
      format = GL_RED;
    else if(nrChannels == 3)
      format = GL_RGB;
    else if(nrChannels == 4)
      format = GL_RGBA;
  
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
    stbi_image_free(data);
    }
    else
    {
      std::cout << "Fehler beim Laden der Textur: " << path << std::endl;
      stbi_image_free(data);
    }
  
    return textureID;
}



int main(int argc, char* argv[])
{
  const unsigned int uiWidth = 800;
  const unsigned int uiHeight = 600;
  //GLFWwindow* pWindow;

  glfwSetErrorCallback(errorCallback);                          // set a callback for GLFW errors

  if(!glfwInit()) {
    std::cerr << "Fehler: GLFW konnte nicht initialisiert werden!" << std::endl;
    return -1;                                    // initialize library
  }
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3);               // open a OpenGL 3.2 context
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow* pWindow = glfwCreateWindow( uiWidth, uiHeight, "Mensch Ärgere Dich Nicht", NULL, NULL); // öffnet das Fenster
  if(!pWindow)
  {
    std::cerr << "Fehler: Fenster konnte nicht erstellt werden!" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(pWindow);                              // make the render context current
  gladLoadGL();                                                 // load all the GL commands
  glfwSwapInterval(1);                                          // synchronize with display update

  // OpenGL aktivieren
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, uiWidth, uiHeight);

  // Board-Objekt erstellen
  g_pcBoard = new Board();

  if (!g_pcBoard) {
        std::cerr << "Fehler: Board konnte nicht erstellt werden!" << std::endl;
        glfwTerminate();
        return -1;
  }

  g_pcBoard->setWindowSize(uiWidth, uiHeight);
  g_pcBoard->initGL();

 // Hintergrund-Quad erstellen
  float quadVertices[] = {
    // Position      // TexCoords
    -1.0f,  1.0f,    0.0f, 1.0f,  // oben links
    -1.0f, -1.0f,    0.0f, 0.0f,  // unten links
    1.0f, -1.0f,    1.0f, 0.0f,  // unten rechts

    -1.0f,  1.0f,    0.0f, 1.0f,  // oben links
    1.0f, -1.0f,    1.0f, 0.0f,  // unten rechts
    1.0f,  1.0f,    1.0f, 1.0f   // oben rechts
  };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
  
    // Hintergrundtextur laden
    bgTexture = loadTexture("textures/background.jpg");
  
    // Hintergrund-Shader kompilieren (Pfad anpassen, falls nötig)
    std::string bgVertPath = "shader/background.vert";
    std::string bgFragPath = "shader/background.frag";
    bgShaderProgram = compileShader(bgVertPath, bgFragPath);
    if (!bgShaderProgram)
    {
        std::cerr << "Fehler: Hintergrund-Shader konnte nicht geladen werden!" << std::endl;
    }



  // Erzeuge 16 Figuren und weise ihnen Position und Farbe zu
  // Definiere Farben:
  glm::vec3 red   = glm::vec3(0.85f, 0.0f, 0.0f);
  glm::vec3 green = glm::vec3(0.0f, 0.70f, 0.0f);
  glm::vec3 yellow= glm::vec3(0.95f, 0.95f, 0.0f);
  glm::vec3 blue  = glm::vec3(0.0f, 0.5f, 0.9f);

  
  // Board ist derzeit ein Quadrat von ca. -0.5 bis 0.5 (skaliert wird im Board-Konstruktor)
  std::vector<glm::vec3> redPositions = {
      glm::vec3(-0.38f,  0.38f, 0.0f), glm::vec3(-0.28f,  0.38f, 0.0f),
      glm::vec3(-0.38f,  0.28f, 0.0f), glm::vec3(-0.28f,  0.28f, 0.0f)
  };
  std::vector<glm::vec3> greenPositions = {
      glm::vec3(0.38f,  -0.38f, 0.0f), glm::vec3(0.28f,  -0.38f, 0.0f),
      glm::vec3(0.38f,  -0.28f, 0.0f), glm::vec3(0.28f,  -0.28f, 0.0f)
  };
  std::vector<glm::vec3> yellowPositions = {
      glm::vec3(-0.38f, -0.38f, 0.0f), glm::vec3(-0.28f, -0.38f, 0.0f),
      glm::vec3(-0.38f, -0.28f, 0.0f), glm::vec3(-0.28f, -0.28f, 0.0f)
  };
  std::vector<glm::vec3> bluePositions = {
      glm::vec3(0.38f, 0.38f, 0.0f), glm::vec3(0.28f, 0.38f, 0.0f),
      glm::vec3(0.38f, 0.28f, 0.0f), glm::vec3(0.28f, 0.28f, 0.0f)
  };

  auto createFiguren = [&](const std::vector<glm::vec3>& positions, const glm::vec3& color) {
    for (const auto &pos : positions) {
        Figur* figur = new Figur();
        // Setze den lokalen Transform als Translation
        glm::mat4 local = glm::translate(glm::mat4(1.0f), pos);
        local = glm::scale(local, glm::vec3(0.1f));  // Skaliere die Figur z. B. um den Faktor 0.3
        figur->setLocalTransform(local);
        figur->setColor(color);
        g_figuren.push_back(figur);
    }
  };

  createFiguren(redPositions, red);
  createFiguren(greenPositions, green);
  createFiguren(yellowPositions, yellow);
  createFiguren(bluePositions, blue);

  // set callback functions
  glfwSetWindowSizeCallback(pWindow, resizeCallback);           // set the callback in case of window resizing
  glfwSetKeyCallback(pWindow, keyboardCallback);                // set the callback for key presses

  std::cout << "press q to quit" << std::endl;
  std::cout << "press k to turn left" << std::endl;
  std::cout << "press l to turn right" << std::endl;
  std::cout << "press a to turn forward" << std::endl;
  std::cout << "press y to turn backward" << std::endl;

  // main loop for rendering and message parsing
  while (!glfwWindowShouldClose(pWindow))                       // Loop until the user closes the window
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);         // Bildschirm leeren
  
   // Hintergrund rendern
    glDisable(GL_DEPTH_TEST);  // Tiefentest deaktivieren, damit das Quad komplett sichtbar ist
    glUseProgram(bgShaderProgram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);
    int bgTexLoc = glGetUniformLocation(bgShaderProgram, "backgroundTexture");
    glUniform1i(bgTexLoc, 0);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);   // Tiefentest wieder aktivieren


    g_pcBoard->render();  // Das Spielfeld rendern!
    
    // Aktualisiere und zeichne alle Figuren
      glm::mat4 boardMatrix = g_pcBoard->getModelMatrix();
      for (auto figur : g_figuren) {
          // Berechne finale Modellmatrix = boardMatrix * (lokaler Transform der Figur)
          figur->setModelMatrix(boardMatrix * figur->getLocalTransform());
          figur->render();
      }

    

    glfwSwapBuffers(pWindow);                                 // swap front and back buffers

    glfwPollEvents();                                         // process events
  }

  
  g_pcBoard->uninitGL();
  
  // Aufräumen
  for (auto figur : g_figuren)
    delete figur;
  delete g_pcBoard;  // Spielfeld löschen

  glfwTerminate();  // end glfw library

  return 0;
}

void errorCallback(int iError, const char* pcDescription) {
  std::cerr << "GLFW Fehler: " << pcDescription << std::endl;
}

void resizeCallback(GLFWwindow* pWindow, int width, int height) {
  if (g_pcBoard) {
    g_pcBoard->setWindowSize(width, height);
  }
}

void keyboardCallback(GLFWwindow* pWindow, int iKey, int iScancode, int iAction, int iMods) {
  if (iAction == GLFW_PRESS || iAction == GLFW_REPEAT) {
    switch (iKey) {
        case GLFW_KEY_Q:
                glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
        break;
        case GLFW_KEY_A: // Rotieren um X-Achse nach oben
          if (g_pcBoard) g_pcBoard->rotX(2.0f);
        break;
        case GLFW_KEY_Z: // Rotieren um X-Achse nach unten
          if (g_pcBoard) g_pcBoard->rotX(-2.0f);
        break;
        case GLFW_KEY_K: // Rotieren um Y-Achse nach links
          if (g_pcBoard) g_pcBoard->rotY(2.0f);
        break;
        case GLFW_KEY_L: // Rotieren um Y-Achse nach rechts
          if (g_pcBoard) g_pcBoard->rotY(-2.0f);
        break;
        default:
          if (g_pcBoard) g_pcBoard->keyPressed(iKey);
        break;
    }
  }
}