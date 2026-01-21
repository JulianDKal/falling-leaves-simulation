#include <iostream>
#include "GL/glew.h"
#include "SDL3/SDL.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "glm/glm.hpp"
#include "Shader.h"
#include "Leaf.h"
#include <cstdlib>
#include <vector>
#include <random>
#include "Random.h"
#include "Camera.h"
#include "Emitter.h"
#include "UI.h"
#include "SDL3/SDL_events.h"

float wWidth = 1920.0f;
float wHeight = 1080.0f;

float gridVertices[] = {
    // Positions          // UVs (texture coordinates)
    -250.0f, 0.0f,  250.0f,  0.0f, 75.0f,  // Top-left
    250.0f, 0.0f,  250.0f,  75.0f, 75.0f,  // Top-right
    250.0f, 0.0f, -250.0f,  75.0f, 0.0f,  // Bottom-right
    -250.0f, 0.0f, -250.0f,  0.0f, 0.0f   // Bottom-left
};

float xAxisVertices[] = {
    -100.0f, 0.01f, 0.0f,
    100.0f, 0.01f, 0.0f
};
float zAxisVertices[] = {
    0.0f, 0.01f, -100.0f,
    0.0f, 0.01f, 100.0f
};

float quadVertices[] = {
    -1.0f, 0.0f, -1.0f,  
     1.0f, 0.0f, -1.0f,  
     1.0f, 0.0f,  1.0f,  
    -1.0f, 0.0f,  1.0f 
};

double currentTime, lastFrameTime = 0.0;
float deltaTime;
bool simulationRunning = false;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_Window* window = SDL_CreateWindow("Falling Leaves Simulation",wWidth, wHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_SetWindowMinimumSize(window, 800, 600);
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (!context) {
        std::cerr << "OpenGL context creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize GLEW after OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    UI ui(window, context);

    glEnable(GL_DEPTH_TEST);

    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    EmitterParams emitterParams {
        glm::vec3(0.0f, 0.0f, 0.0f),  // windForce
        0.6f,                          // size
        9.81f,                         // gravity
        false,                         // spiralingMotion
        false,                         // tumbling
        1000,                         // leafCount
        10.0f,                         // emitRadius
        15.0f,                         // emitHeight
        EmitterShape::circleShape,      // shape of the emitter
        ParticleShape::sphereShape     // particle shape
    };

    std::vector<glm::vec3>* circleVector = generateCirclePoints(24);

    Shader gridShader;
    gridShader.createProgram("./../shaders/grid_vertex.glsl", "./../shaders/grid_fragment.glsl");
    Shader lineShader;
    lineShader.createProgram("./../shaders/line_vertex.glsl", "./../shaders/line_fragment.glsl");

    Texture gridTexture;
    gridTexture.initialize("./../textures/grid.jpg", 0);

    Camera cam;

    Emitter emitter(emitterParams);    

    //Grid object setup
    unsigned int grid_VBO, grid_VAO;
    glGenVertexArrays(1, &grid_VAO);
    glGenBuffers(1, &grid_VBO);

    glBindVertexArray(grid_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, grid_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //Axis OBjects setup
    unsigned int xAxis_VBO, xAxis_VAO;
    glGenVertexArrays(1, &xAxis_VAO);
    glGenBuffers(1, &xAxis_VBO);

    glBindVertexArray(xAxis_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, xAxis_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(xAxisVertices), xAxisVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    unsigned int zAxis_VBO, zAxis_VAO;
    glGenVertexArrays(1, &zAxis_VAO);
    glGenBuffers(1, &zAxis_VBO);

    glBindVertexArray(zAxis_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, zAxis_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(zAxisVertices), zAxisVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Circle Shape Object setup
    unsigned int circleVBO, circleVAO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);

    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, circleVector->size() * 3 * sizeof(float), circleVector->data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Quad Shape Object Setup
    unsigned int quadVBO, quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    bool running = true;
    float rotationSpeed = 0.3f;

    lastFrameTime = SDL_GetPerformanceCounter();

    while (running)
    {

        //Calculate delta time
        currentTime = SDL_GetPerformanceCounter();
        //SDL_GetPerformanceFrequency is the resolution of the performance counter, 
        //could for example be 1 million for microseconds, 1 billion for nanoseconds etc.
        deltaTime = (currentTime - lastFrameTime) / SDL_GetPerformanceFrequency(); 
        lastFrameTime = currentTime;

        //std::cout << deltaTime << " " << deltaTime / SDL_GetPerformanceFrequency() << " " << SDL_GetPerformanceFrequency() << std::endl;
        // std::cout << START_SIMULATION_EVENT << " " << STOP_SIMULATION_EVENT << std::endl;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui_ImplSDL3_ProcessEvent(&event);

            if(event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            if(event.type == SDL_EVENT_WINDOW_RESIZED){
                wWidth = event.window.data1;
                wHeight = event.window.data2;
                glViewport(0, 0, wWidth, wHeight);
            }
            //Don't react to clicks if they are already being handled by imgui
            if(io.WantCaptureMouse) {
                continue;
            }
            //Pan right and left with the mouse and rotate around the object
            else if(event.type == SDL_EVENT_MOUSE_MOTION){
                if(event.motion.state == SDL_BUTTON_LEFT){
                    cam.rotate(event.motion.xrel * 0.005f, event.motion.yrel * 0.005f);
                }
            }
            //Zoom in and out
            else if(event.type == SDL_EVENT_MOUSE_WHEEL) {
                cam.zoom(event.wheel.y * 0.4f);
            }
        }

        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cam.update();

        glm::mat4 view = cam.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 250.0f);

        ui.update(emitterParams);

        while (SDL_PollEvent(&event)) {
            if(event.type == START_SIMULATION_EVENT) {
                std::cout << "Start the simulation event received!" << std::endl;
                simulationRunning = true;
            }
            else if(event.type == STOP_SIMULATION_EVENT) {
                std::cout << "Stop the simulation event received!" << std::endl;
                simulationRunning = false;
            }
            else if(event.type == PARTICLE_COUNT_UPDATED_EVENT) {
                emitter.resizeParticleCount(emitterParams);
            }
            else if(event.type == EMIT_AREA_CHANGED_EVENT) {
                emitter.changeEmitArea(emitterParams);
            }
        }

        glm::mat4 model = glm::mat4(1.0f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        glBindVertexArray(grid_VAO);
        glUseProgram(gridShader.ID);
        gridShader.useTexture(gridTexture, "gridTexture");
        gridShader.setMatrix4("model", model);
        gridShader.setMatrix4("view", view);
        gridShader.setMatrix4("projection", projection);
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glm::vec3 xColor = {1.0f, 0.0f, 0.0f};
        glm::vec3 zColor = {0.0f, 0.0f, 1.0f};

        glLineWidth(3.0f);

        //Draw x and z Axis
        glBindVertexArray(xAxis_VAO);
        glUseProgram(lineShader.ID);
        lineShader.setVec3f("color", xColor);
        lineShader.setMatrix4("model", model);
        lineShader.setMatrix4("view", view);
        lineShader.setMatrix4("projection", projection);
        glDrawArrays(GL_LINES, 0, 2);

        glBindVertexArray(zAxis_VAO);
        lineShader.setVec3f("color", zColor);
        glDrawArrays(GL_LINES, 0, 2);

        //Draw the Gizmos Shape
        glLineWidth(3.0f);
        model = glm::translate(model, glm::vec3 {0, emitterParams.emitHeight, 0});
        model = glm::scale(model, glm::vec3 {emitterParams.emitRadius});
        lineShader.setMatrix4("model", model);
        lineShader.setVec3f("color", xColor);

        if(emitterParams.shape == EmitterShape::circleShape) {
            glBindVertexArray(circleVAO);
            glDrawArrays(GL_LINE_LOOP, 0, circleVector->size());

        }
        else if(emitterParams.shape == EmitterShape::boxShape){
            glBindVertexArray(quadVAO);
            glDrawArrays(GL_LINE_LOOP, 0, sizeof(quadVertices) / 3 / 4);
        }

        glLineWidth(1.0f);
        glDisable(GL_BLEND);

        //Actually draw all the leaves
        if(simulationRunning) {
            emitter.update(deltaTime, emitterParams);
            emitter.draw(view, projection, emitterParams);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

    }
    

    return 0;
}