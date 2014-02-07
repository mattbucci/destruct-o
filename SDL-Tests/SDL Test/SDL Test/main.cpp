//
//  main.cpp
//  SDL Test
//
//  Created by Nathaniel Lewis on 1/27/14.
//  Copyright (c) 2014 HoodooNet. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_filesystem.h>

#include <SDL2_image/SDL_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <thread>

#include "Buffer.hpp"
#include "Program.hpp"
#include "cube.h"
#include "util.hpp"

// global variable defining if the application should die
bool done = false;

// Called when SDL receives an event
int EventFilter(void* userdata, SDL_Event* event)
{
    // Check if we've receivd the quit event
    if ( event->type == SDL_QUIT )
    {
        // Cause the main loop to exit the application
        done = true;
    }
    
    // Check if we've received a key down event
    else if ( event->type == SDL_KEYDOWN )
    {
        // Check if the depressed key is the escape key
        if ( event->key.keysym.sym == SDLK_ESCAPE )
        {
            // Cause the main loop to exit the application
            done = true;
        }
    }
    
    // Check if the window was resized
    else if (event->type == SDL_WINDOWEVENT)
    {
        switch (event->window.event)
        {
            case SDL_WINDOWEVENT_RESIZED:
            {
                SDL_Log("Window %d resized to %dx%d", event->window.windowID, event->window.data1, event->window.data2);
                
                break;
            }
        }
    }
    
    // Successfully listened to events
    return 0;
}

// Render thread
int RenderThread(void* userdata)
{
    // Cast the user data to an SDL window
    SDL_Window *window = static_cast<SDL_Window *>(userdata);
    
    // Request double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Request a 32 bit depth buffer
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    
    // Request 32 bit color buffer (RGBA8888)
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    
    // Request OpenGL 3.2
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    // Attempt to create an OpenGL context for this window
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    // If the OpenGL context failed to be created, log the error and exit
    if ( !context )
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create OpenGL context: %s\n", SDL_GetError());
        
        // Fix me - This is BAD FORM to just force quit from some random thread.  Possibly pass a pointer to the gl context
        SDL_Quit();
        return 2;
    }
    
    // Turn on Vsync
    SDL_GL_SetSwapInterval(1);
    
    // Set the standard clear color for OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    // Set the perspective matrix
    float FoV    = 60.0f;
    float aspect = 800.0f / 600.0f;
    float zNear  = 0.1f;
    float zFar   = 500.0f;
    
    // Create the model-view-projection matrix
    glm::mat4 projection = glm::perspective(FoV, aspect, zNear, zFar);
    
    // Get the shader
    OpenWorlds::GL::Program shader(defaultResourceDirectory() + "/Shader.vsh", defaultResourceDirectory() + "/Shader.fsh");
    
    // Get uniform locations
    GLuint viewProjectUniform = glGetUniformLocation(shader.nativeHandle(), "VP");
    GLuint textureUniform     = glGetUniformLocation(shader.nativeHandle(), "Texture");
    
    // Generate a vertex array object for our cube
    GLuint cube;
    glGenVertexArrays(1, &cube);
    glBindVertexArray(cube);
    
    // Allocate some OpenGL buffers
    GLuint buffers[5];
    glGenBuffers(5, buffers);
    
    // Load the verticies of the cube
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verticies), cube_verticies, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
    
    // Load the normals of the cube
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_normals), cube_normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), 0);
    
    // Load the texture coordinates of the cube
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_texcoords), cube_texcoords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), 0);

    // MVP matricies
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * 16) * 100000, NULL, GL_DYNAMIC_DRAW);
    for(int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (const GLvoid *)(sizeof(float) * i * 4));
        glVertexAttribDivisor(3 + i, 1);
    }
    
    // Load the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);
    
    // Unbind vertex array
    glBindVertexArray(0);
    
    // Initial state
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    for(int i = -100; i <= 100; i++)
    {
        for(int j = -100; j <= 100; j++)
        {
            // Build the MVP matrix
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 1.1, 0, j * 1.1));
            
            // Substitute the date
            glBufferSubData(GL_ARRAY_BUFFER, ((201 * (j + 100)) + (i + 100)) * (sizeof(float) * 16), (sizeof(float) * 16), &model[0][0]);
        }
    }
    
    // Variable to keep track of immediate fps
    Uint32 frameCounter = 0;
    Uint32 priorTicks = SDL_GetTicks();
    float  angle = 0.0f;
    
    // Swap windows
    while( !done )
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
        glBindVertexArray(cube);
        
        // Use the program
        glUseProgram(shader.nativeHandle());
        
        // Draw the cube
        glDrawElementsInstanced(GL_TRIANGLES, sizeof(cube_elements)/sizeof(cube_elements[0]), GL_UNSIGNED_BYTE, 0, 201 * 201);
        
        // Swap screen buffers
        SDL_GL_SwapWindow(window);
        
        // Increment the frame counter
        frameCounter++;
        angle += (1.0f / 2.0f) * ((M_PI) / 180.0f);
    }
    
    // Success
    return 0;
}

int main(int argc, const char * argv[])
{
    // Enable standard application logging
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    
    // Initialize SDL for video output
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    // Attempt to an OpenGL enabled SDL window (640 x 480)
    SDL_Window *window = SDL_CreateWindow( "SDL OpenGL Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL /*| SDL_WINDOW_RESIZABLE*/);
    
    // If the window failed to be created, log the error and exit
    if ( !window )
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to create OpenGL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 2;
    }
    
    // Schedule an event listener
    SDL_AddEventWatch(EventFilter, NULL);
    
    // Schedule a render thread
    SDL_Thread *renderThread = SDL_CreateThread(RenderThread, "Rendering Thread", static_cast<void *>(window));
    
    // Loop while the application desires to execute
    while ( ! done )
    {
        // Wait for SDL events ... forever
        SDL_PumpEvents();
        
        // SDL is stupid and does not block on streams.  So pump events will cause an insane amount of looping
        // limit the pump event rate to 100 times a second
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Wait for the render thread to complete its work
    SDL_WaitThread(renderThread, NULL);
    
    // Quit the applicaiton
    SDL_Quit();
    
    // Return success
    return 0;
}
