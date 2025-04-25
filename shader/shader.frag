//#version 330 core
//in vec2 TexCoords;  // Texture-Koordinaten vom Vertex-Shader
//out vec4 FragColor; // Ausgabe-Farbe

//uniform sampler2D texture1; // Textur-Sampler

//void main()
//{
//    FragColor = texture(texture1, TexCoords); // Textur abrufen
//}

#version 330 core

// Eingangsvariable für Texturkoordinaten (wird nur benötigt, wenn Textur verwendet wird)
in vec2 TexCoords;

// Uniforms
uniform sampler2D texture1; // Für das Board
uniform bool useTexture;    // Schaltet zwischen Textur und Farbe um
uniform vec3 objectColor;   // Für die Figuren

// Ausgabe der Fragmentfarbe
out vec4 FragColor;

void main()
{
    if(useTexture)
    {
        // Wenn useTexture true ist, wird die Textur verwendet (z. B. für das Board)
        FragColor = texture(texture1, TexCoords);
    }
    else
    {
        // Andernfalls wird die Objektfarbe verwendet (z. B. für die Figuren)
        FragColor = vec4(objectColor, 1.0);
    }
}