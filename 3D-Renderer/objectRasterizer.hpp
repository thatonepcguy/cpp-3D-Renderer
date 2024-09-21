#pragma once
#include "main.hpp"
#include <SDL2/SDL.h>

typedef struct {
    int x;
    int y;
} Vector2Int;

void rasterizeObject(Object3D object, SDL_Renderer* renderer);