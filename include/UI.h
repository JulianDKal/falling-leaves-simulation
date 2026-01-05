#pragma once
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "SDL3/SDL.h"
#include "glm/glm.hpp"
#include "Helpers.h"

extern float wWidth;
extern float wHeight;

class UI
{
private:
    bool show_demo_window = false, show_another_window = false;
public:
    UI(SDL_Window* window, SDL_GLContext context);
    void update(EmitterParams& emitterParams);
    void draw();
    ~UI();
};
