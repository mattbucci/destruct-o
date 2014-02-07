#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <memory>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Program.hpp"

#include "cube.h"
#include "util.hpp"

using namespace std;

class Graphics
{
private:
    
    SDL_Window*   _window;
    SDL_GLContext _context;
    
    // View matrices
    glm::mat4     projection;
    
    // Buffers
    GLuint        cube;
    GLuint        buffers[5];
    
    // Shader
    OpenWorlds::GL::Program *shader;
    
    // Uniforms
    GLuint        viewProjectUniform;
    
    // Variable to keep track of immediate fps
    Uint32 frameCounter;
    Uint32 priorTicks;
    float  angle;
    
public:
    
    Graphics(SDL_Window* window, SDL_DisplayMode& displayMode) : _window(window)
    {
        // Request double buffering
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        
        // Request a 32 bit depth buffer
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
        
        // Request 32 bit color buffer (RGBA8888)
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        
        // Request OpenGL ES 2.0
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        
        // Attempt to create an OpenGL context for this window
        _context = SDL_GL_CreateContext(window);
        
        // If the OpenGL context failed to be created, log the error and exit
        if ( !_context )
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create OpenGL context: %s\n", SDL_GetError());
            SDL_Quit();
        }
        
        // Set the standard clear color for OpenGL
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glViewport(0, 0, displayMode.w, displayMode.h);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        
        // Compile the shader
        shader = new OpenWorlds::GL::Program(defaultResourceDirectory() + "/Shader.vsh", defaultResourceDirectory() + "/Shader.fsh");
        viewProjectUniform = glGetUniformLocation(shader->nativeHandle(), "VP");
        
        // Set the perspective matrix
        float FoV    = 75.0f;
        float aspect = static_cast<float>(displayMode.w) / static_cast<float>(displayMode.h);
        float zNear  = 0.1f;
        float zFar   = 500.0f;
        
        // Create the model-view-projection matrix
        projection = glm::perspective(FoV, aspect, zNear, zFar);
        
        // Create the vertex array object and the buffers
        glGenVertexArraysOES(1, &cube);
        glGenBuffers(5, buffers);
        
        // Bind the Vertex array object
        glBindVertexArrayOES(cube);
        
        // Load the verticies of the cube
        GLuint attribute_verticies = glGetAttribLocation(shader->nativeHandle(), "Position");
        glEnableVertexAttribArray(attribute_verticies);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verticies), cube_verticies, GL_STATIC_DRAW);
        glVertexAttribPointer(attribute_verticies, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
        
        // Load the normals of the cube
        GLuint attribute_normals = glGetAttribLocation(shader->nativeHandle(), "NormalIn");
        glEnableVertexAttribArray(attribute_normals);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
        glVertexAttribPointer(attribute_normals, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
        
        // Load the texture coordinates of the cube
        GLuint attribute_texcoords = glGetAttribLocation(shader->nativeHandle(), "UVIn");
        glEnableVertexAttribArray(attribute_texcoords);
        glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(attribute_texcoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);
        
        // MVP matricies
        GLuint attribute_m = glGetAttribLocation(shader->nativeHandle(), "M");
        glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
        glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 16) * (201 * 201), NULL, GL_DYNAMIC_DRAW);
        for(int i = 0; i < 4; i++)
        {
            glEnableVertexAttribArray(attribute_m + i);
            glVertexAttribPointer(attribute_m + i, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (const GLvoid *)(sizeof(float) * i * 4));
            glVertexAttribDivisorEXT(attribute_m + i, 1);
        }
        
        // Load the index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[4]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
        
        // Unbind vertex array
        glBindVertexArrayOES(0);
        
        // Initial state
        glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
        for(int i = -50; i <= 50; i++)
        {
            for(int j = -50; j <= 50; j++)
            {
                // Build the MVP matrix
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.1, 0, j * 1.1));
                
                // Substitute the date
                glBufferSubData(GL_ARRAY_BUFFER, ((101 * (j + 50)) + (i + 50)) * (sizeof(float) * 16), (sizeof(float) * 16), &model[0][0]);
            }
        }
        
        // Initialize
        frameCounter = 0;
        priorTicks = SDL_GetTicks();
        angle = 0.0f;
    }
    
    ~Graphics()
    {
        SDL_GL_DeleteContext(_context);
    }
    
    void update()
    {
        // Frame time counter
        Uint32 currentTicks = SDL_GetTicks();
        if(currentTicks - priorTicks >= 1000)
        {
            // Calculate the frame time
            SDL_Log("fps = %f", static_cast<float>(frameCounter) / (static_cast<float>(currentTicks - priorTicks) * 0.001f));
            
            // Get new ticks
            priorTicks = currentTicks;
            frameCounter = 0;
        }
        
        // Update the viewProjection matrix
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(10.0f * sin(angle), 0.0f, 10.0f * cos(angle)), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 VP = projection * view;
        
        // Update the view projection matrix in the shader
        glUniformMatrix4fv(viewProjectUniform, 1, GL_FALSE, &VP[0][0]);
        
        // Clear the OpenGL buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        // Use a vertex array object
        glBindVertexArrayOES(cube);
        
        // Use the program
        glUseProgram(shader->nativeHandle());
        
        // Draw the cube
        glDrawElementsInstancedEXT(GL_TRIANGLES, sizeof(cube_elements)/sizeof(cube_elements[0]), GL_UNSIGNED_BYTE, 0, 101 * 101);
        
        // Swap screen buffers
        SDL_GL_SwapWindow(_window);
        
        // Increment the frame counter
        frameCounter++;
        angle += (1.0f / 2.0f) * ((M_PI) / 180.0f);
    }
};

void UpdateFrame(void* param)
{
    Graphics* graphics = (Graphics*)param;
    graphics->update();
}

int EventFilter(void* userdata, SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_FINGERMOTION:
            SDL_Log("Finger Motion");
            return 0;
            
        case SDL_FINGERDOWN:
            SDL_Log("Finger Down");
            return 0;
            
        case SDL_FINGERUP:
            SDL_Log("Finger Up");
            return 0;
    }
    
    return 1;
}


int main(int argc, char *argv[])
{
    /* initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Could not initialize SDL\n");
        return 1;
    }
    
    SDL_DisplayMode displayMode;
    SDL_GetDesktopDisplayMode(0, &displayMode);
    SDL_Log("Display dimensions = %d,%d", displayMode.w, displayMode.h);
    
    /* create window and renderer */
    SDL_Window* window = SDL_CreateWindow(NULL, 0, 0, displayMode.w, displayMode.h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        printf("Could not initialize Window\n");
        return 1;
    }
    
    auto gl = SDL_GL_CreateContext(window);
    
    unique_ptr<Graphics> graphics = unique_ptr<Graphics>(new Graphics(window, displayMode));
    
    SDL_iPhoneSetAnimationCallback(window, 1, UpdateFrame, graphics.get());
    SDL_AddEventWatch(EventFilter, NULL);
    
    //Game Loop
    SDL_Event event;
    auto done = false;
    while (!done)
    {
        SDL_PumpEvents();
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    done = true;
                    break;
                    
                case SDL_APP_DIDENTERFOREGROUND:
                    SDL_Log("SDL_APP_DIDENTERFOREGROUND");
                    break;
                    
                case SDL_APP_DIDENTERBACKGROUND:
                    SDL_Log("SDL_APP_DIDENTERBACKGROUND");
                    break;
                    
                case SDL_APP_LOWMEMORY:
                    SDL_Log("SDL_APP_LOWMEMORY");
                    break;
                    
                case SDL_APP_TERMINATING:
                    SDL_Log("SDL_APP_TERMINATING");
                    break;
                    
                case SDL_APP_WILLENTERBACKGROUND:
                    SDL_Log("SDL_APP_WILLENTERBACKGROUND");
                    break;
                    
                case SDL_APP_WILLENTERFOREGROUND:
                    SDL_Log("SDL_APP_WILLENTERFOREGROUND");
                    break;
                    
                case SDL_WINDOWEVENT:
                {
                    switch (event.window.event)
                    {
                        case SDL_WINDOWEVENT_RESIZED:
                        {
                            SDL_Log("Window %d resized to %dx%d", event.window.windowID, event.window.data1, event.window.data2);
                            
                            break;
                        }
                    }
                }
            }
        }
    }
    
    SDL_GL_DeleteContext(gl);
    
    // Done! Close the window, clean-up and exit the program.
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}
