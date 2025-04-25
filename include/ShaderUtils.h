#ifndef SHADERUTILS_H
#define SHADERUTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Inline-Funktion: Diese Definition wird in jede Übersetzungseinheit eingebunden,
// ohne dass es zu doppelten Symbolen kommt.
inline std::string readShaderFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "Fehler: Shader-Datei konnte nicht geöffnet werden: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::cout << "Shader-Datei erfolgreich geladen: " << filePath << std::endl;
    return buffer.str();
}

#endif 