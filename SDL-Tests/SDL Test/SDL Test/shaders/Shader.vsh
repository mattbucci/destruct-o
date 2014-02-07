#version 330 core

// Position on the vertex
layout (location = 0) in vec4 Position;

// Normal of the vertex
layout (location = 1) in vec4 NormalIn;

// Texture coordinate
layout (location = 2) in vec2 UVIn;

// The model matrix
layout (location = 3) in mat4 M;

// This is the view projection matrix, as its a lot more likely to change than the model matrix
uniform mat4 VP;

// Output vertex normal
out vec4 Normal;

// Output texture coordinate to the next shader stage
out vec2 UV;

// Main method
void main (void)
{
    // Set the varying vector for the UV
    UV = UVIn;
    
    // Set the varying vector for the Normal
    Normal = NormalIn;
    
    // Position is the projection matrix multiplied by the position
    gl_Position = VP * M * Position;
}