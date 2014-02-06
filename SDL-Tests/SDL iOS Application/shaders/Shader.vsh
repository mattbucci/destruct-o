// Position on the vertex
attribute vec4 Position;

// Normal of the vertex
attribute vec4 NormalIn;

// Texture coordinate
attribute vec2 UVIn;

// Model view matrix
attribute mat4 MVP;

// The model - view - project matrix, doesn't make sense to do tons of un-necessary math
//uniform mat4 MVP;

// Output vertex normal
varying highp vec4 Normal;

// Output texture coordinate to the next shader stage
varying highp vec2 UV;

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