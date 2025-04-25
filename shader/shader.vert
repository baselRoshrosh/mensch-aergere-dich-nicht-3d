#version 330 core
layout (location = 0) in vec3 aPos;       // Vertex Position
layout (location = 1) in vec2 aTexCoord;  // Texture Coordinates

out vec2 TexCoords;  // Texture-Koordinaten für Fragment-Shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); // Transformierte Position
    TexCoords = aTexCoord;  // Texture-Koordinaten weitergeben
}