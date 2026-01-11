#pragma once
#include <SDL3/SDL.h>

const inline Uint32 START_SIMULATION_EVENT {SDL_RegisterEvents(1)};
const inline Uint32 STOP_SIMULATION_EVENT {SDL_RegisterEvents(1)};
const inline Uint32 PARTICLE_COUNT_UPDATED_EVENT {SDL_RegisterEvents(1)};