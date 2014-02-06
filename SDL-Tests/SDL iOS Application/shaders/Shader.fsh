// Input texture coordinate
varying highp vec2 UV;

// Input normal from the vertex stage
varying highp vec4 Normal;

// Texture unit to use
uniform sampler2D Texture;

// Main shader method
void main (void)
{
    // Passthrough color
    //FinalColor = vec4(0.5, 0.5, 0.5, 1.0) * texture(Texture, UV);
    highp vec4 FinalColor;
    if(Normal.x > 0.0)
    {
        FinalColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else if(Normal.x < 0.0)
    {
        FinalColor = vec4(0.0, 1.0, 0.0, 1.0);
    }
    else if(Normal.y > 0.0)
    {
        FinalColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else if(Normal.y < 0.0)
    {
        FinalColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
    else if(Normal.z > 0.0)
    {
        FinalColor = vec4(1.0, 0.0, 1.0, 1.0);
    }
    else if(Normal.z < 0.0)
    {
        FinalColor = vec4(0.0, 1.0, 1.0, 1.0);
    }
    gl_FragColor = FinalColor;
}