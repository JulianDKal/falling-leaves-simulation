#include "UI.h"


UI::UI(SDL_Window* window, SDL_GLContext context)
{
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
}

void UI::update(EmitterParams& emitterParams)
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2{wWidth / 5, wHeight});
    ImGui::SetNextWindowPos(ImVec2{0, 0});
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4{0.0f, 0.0f, 0.0f, 0.94f});  // Black with slight transparency
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(69/255.0f, 69/255.0f, 138/255.0f, 0.94f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(89/255.0f, 89/255.0f, 158/255.0f, 0.94f));

    ImGui::Begin("Controls");                        

    ImGui::Text("%.1f FPS", io.Framerate);
    ImGui::Text("Application average %.3f ms/frame", 1000.0f / io.Framerate);
    ImGui::Dummy(ImVec2{0, 20});

    //#######################################################################################################################################
    //EMITTER SETTINGS
    //#######################################################################################################################################

    
    // Section header with spacing
    ImGui::SeparatorText("Emitter Parameters");
    ImGui::Spacing();

    // 1. Wind Force (vec3 with proper labeling)
    ImGui::Text("Wind Force:");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::DragFloat3("##windForce", &emitterParams.windForce.x, 0.1f, -10.0f, 10.0f, "%.1f");
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::SetTooltip("Wind direction and strength\nX, Y, Z components");
    }

    ImGui::Spacing();

    ImGui::Text("Gravity:");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);
    
    if (ImGui::InputFloat("##Gravity", &emitterParams.gravity, 1.0f, 100.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue)) {
    // Validate when Enter is pressed
        emitterParams.gravity = glm::clamp(emitterParams.gravity, 1.0f, 100.0f);
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(%f)", emitterParams.gravity);


    ImGui::Spacing();

    // ImGui::Text("Motion:");
    // float buttonWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
    // if (ImGui::Button(emitterParams.spiralingMotion ? "Spiraling: ON" : "Spiraling: OFF", 
    //                 ImVec2(buttonWidth, 0))) {
    //     emitterParams.spiralingMotion = !emitterParams.spiralingMotion;
    // }
    // ImGui::SameLine();
    // if (ImGui::Button(emitterParams.tumbling ? "Tumbling: ON" : "Tumbling: OFF", 
    //                 ImVec2(buttonWidth, 0))) {
    //     emitterParams.tumbling = !emitterParams.tumbling;
    // }

    // ImGui::Spacing();

    ImGui::Text("Particle Count:");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.6f);

    if (ImGui::InputInt("##particleCount", &emitterParams.leafCount, 100, 1000, ImGuiInputTextFlags_EnterReturnsTrue)) {
        // Validate when Enter is pressed
        emitterParams.leafCount = glm::clamp(emitterParams.leafCount, 1, 10000000);
        SDL_Event event {.type = PARTICLE_COUNT_UPDATED_EVENT};
        SDL_PushEvent(&event);
    }

    // Display actual count
    ImGui::SameLine();
    ImGui::TextDisabled("(%d)", emitterParams.leafCount);

    ImGui::Spacing();

    // 5. Emit Radius
    ImGui::Text("Emit Radius:");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::SliderFloat("##emitRadius", &emitterParams.emitRadius, 0.1f, 100.0f, "%.1f m");

    if(ImGui::IsItemDeactivatedAfterEdit()) {
        SDL_Event event {.type = EMIT_AREA_CHANGED_EVENT}; 
        SDL_PushEvent(&event);
    }
    ImGui::PopItemWidth();

    ImGui::Spacing();

    // 6. Emit Height
    ImGui::Text("Emit Height:");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::SliderFloat("##emitHeight", &emitterParams.emitHeight, 0.1f, 200.0f, "%.1f m");
    ImGui::PopItemWidth();

    ImGui::Spacing();

    ImGui::Text("Emitter Shape:");
    ImGui::Spacing();
    ImGui::Indent(10.0f);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    if (ImGui::RadioButton("Circle", emitterParams.shape == EmitterShape::circleShape)) {
        emitterParams.shape = EmitterShape::circleShape;
        SDL_Event event {.type = EMIT_AREA_CHANGED_EVENT}; 
        SDL_PushEvent(&event);

    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Box", emitterParams.shape == EmitterShape::boxShape)) {
        emitterParams.shape = EmitterShape::boxShape;
        SDL_Event event {.type = EMIT_AREA_CHANGED_EVENT}; 
        SDL_PushEvent(&event);

    }
    ImGui::PopStyleVar();
    ImGui::Unindent(10.0f);
    ImGui::Spacing();

    //#######################################################################################################################################
    //PARTICLE SETTINGS
    //#######################################################################################################################################


    // Section header with spacing
    ImGui::Spacing();
    ImGui::SeparatorText("Particle Parameters");
    ImGui::Spacing();

    ImGui::Text("Particle Size:");
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::SliderFloat("##size", &emitterParams.size, 0.1f, 10.0f, "%.1f");
    ImGui::PopItemWidth();

    ImGui::Spacing();


    ImGui::Text("Particle Shape:");
    ImGui::Spacing();
    ImGui::Indent(10.0f);
    if (ImGui::RadioButton("Leaf", emitterParams.particleShape == ParticleShape::leafShape)) {
        emitterParams.particleShape = ParticleShape::leafShape;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Sphere", emitterParams.particleShape == ParticleShape::sphereShape)) {
        emitterParams.particleShape = ParticleShape::sphereShape;   
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Point", emitterParams.particleShape == ParticleShape::pointShape)) {
        emitterParams.particleShape = ParticleShape::pointShape;
    }

    //########################################################################################################################################
    //BLACK HOLE SETTINGS
    //#######################################################################################################################################


    // Section header with spacing
    ImGui::Spacing();
    ImGui::SeparatorText("Black Hole Parameters");
    ImGui::Spacing();

    ImGui::Text("Mass:");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::SliderFloat("##blackHoleMass", &emitterParams.blackHoleMass, 0.1f, 100.0f, "%.1f");
    ImGui::PopItemWidth();

    ImGui::Text("Speed:");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::SliderFloat("##blackHoleSpeed", &emitterParams.blackHoleSpeed, 0.0f, 5.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::Text("Radius:");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::SliderFloat("##blackHoleRadius", &emitterParams.blackHoleRadius, 0.5f, 20.0f, "%.1f");
    ImGui::PopItemWidth();

    ImGui::Text("Angle:");

    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
    ImGui::SliderFloat("##blackHoleAngle", &emitterParams.blackHoleAngle, 0.0f, 90.0f, "%5.0f");
    ImGui::PopItemWidth();


    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();


    if (ImGui::Button("Reset to Defaults", ImVec2(-1, 0))) {
        if(emitterParams.shape != EmitterShape::circleShape || emitterParams.size != 20.0f){
            SDL_Event event {.type = EMIT_AREA_CHANGED_EVENT}; 
            SDL_PushEvent(&event);
        }
        if(emitterParams.leafCount != 10000) {
            SDL_Event event {.type = PARTICLE_COUNT_UPDATED_EVENT}; 
            SDL_PushEvent(&event);
        }
        emitterParams = EmitterParams {
        glm::vec3(0.0f, 0.0f, 0.0f),  // windForce
        std::vector<glm::vec3>(),       //black hole positions
        10.0f,                          //black hole mass
        1.0f,                          //black hole speed
        1.0f,                          //black hole radius
        0.0f,
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
    }

    ImGui::Spacing();
    float width = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
    if(ImGui::Button("Start", ImVec2{width, 0})){
        SDL_Event event {.type = START_SIMULATION_EVENT}; 
        // Push the event to SDLs event queue
        SDL_PushEvent(&event);
    }
    ImGui::SameLine();
    if(ImGui::Button("Stop", ImVec2{width, 0})){
        SDL_Event event {.type = STOP_SIMULATION_EVENT}; 
        // Push the event to SDLs event queue
        SDL_PushEvent(&event);
    }

    ImGui::Checkbox("Demo Window", &show_demo_window);
    ImGui::Checkbox("Another Window", &show_another_window);
    ImGui::End();

    if(show_demo_window) ImGui::ShowDemoWindow();


    ImGui::PopStyleColor(3);
    ImGui::Render();
}

UI::~UI()
{
}