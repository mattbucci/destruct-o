#version 330 core

// Position on the vertex
layout (location = 0) in vec4 Position;

// Normal of the vertex
layout (location = 1) in vec4 NormalIn;

// Texture coordinate
layout (location = 2) in vec2 UVIn;

// Model view matrix
layout (location = 3) in mat4 MVP;

// The model - view - project matrix, doesn't make sense to do tons of un-necessary math
//uniform mat4 MVP;

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
    gl_Position = MVP * Position;
}