#version 330 core

// Texture unit to use
uniform sampler2D Texture;

// Input texture coordinate
in vec2 UV;

// Input normal from the vertex stage
in vec4 Normal;

// Output color
out vec4 FinalColor;

// Main shader method
void main (void)
{
    // Passthrough color
    //FinalColor = vec4(0.5, 0.5, 0.5, 1.0) * texture(Texture, UV);
    if(Normal.x > 0)
    {
        FinalColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else if(Normal.x < 0)
    {
        FinalColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else if(Normal.y > 0)
    {
        FinalColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else if(Normal.y < 0)
    {
        FinalColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
    else if(Normal.z > 0)
    {
        FinalColor = vec4(1.0, 0.0, 1.0, 1.0);
    }
    else if(Normal.z < 0)
    {
        FinalColor = vec4(0.0, 1.0, 1.0, 1.0);
    }
}