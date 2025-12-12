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

double currentTime, lastFrameTime = 0.0;
float deltaTime;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Setup ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init("#version 330");

    //enable transparency for leaf texture
    glEnable(GL_BLEND);
    //glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    Shader gridShader;
    gridShader.createProgram("./../shaders/grid_vertex.glsl", "./../shaders/grid_fragment.glsl");
    Shader lineShader;
    lineShader.createProgram("./../shaders/line_vertex.glsl", "./../shaders/line_fragment.glsl");

    Texture gridTexture;
    gridTexture.initialize("./../textures/grid.jpg", 0);

    Camera cam;
    Emitter emitter(10000);    

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
    
    bool running = true;
    float rotationSpeed = 0.3f;

    while (running)
    {
        //Calculate delta time
        currentTime = SDL_GetPerformanceCounter();
        //SDL_GetPerformanceFrequency is the resolution of the performance counter, could for example be 1 million for microseconds
        deltaTime = (currentTime - lastFrameTime) / SDL_GetPerformanceFrequency(); 
        lastFrameTime = currentTime;

        //std::cout << deltaTime << " " << deltaTime / SDL_GetPerformanceFrequency() << " " << SDL_GetPerformanceFrequency() << std::endl;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if(event.type == SDL_EVENT_QUIT) {
                running = false;
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

        //acc, speed, pos
        //AddForce(10) --> acc geupdated
        //speed += acc * deltaTime;
        //pos += speed * deltaTime;


        glClearColor(1, 1, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplSDL3_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        if(show_demo_window) ImGui::ShowDemoWindow();

        cam.update();

        glm::mat4 view = cam.getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f/720.0f, 0.1f, 100.0f);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }
        ImGui::Render();

        glm::mat4 model = glm::mat4(1.0f);

        glBindVertexArray(grid_VAO);
        glUseProgram(gridShader.ID);
        gridShader.useTexture(gridTexture, "gridTexture");
        gridShader.setMatrix4("model", model);
        gridShader.setMatrix4("view", view);
        gridShader.setMatrix4("projection", projection);
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glm::vec3 xColor = {1.0f, 0.0f, 0.0f};
        glm::vec3 zColor = {0.0f, 0.0f, 1.0f};

        glLineWidth(4.0f);

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

        glLineWidth(1.0f);

        float time = SDL_GetTicks() / 1000.0f;
        emitter.setTimeUniform(time);

        //Actually draw all the leaves
        emitter.update(deltaTime);
        emitter.draw(view, projection);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

    }
    

    return 0;
}