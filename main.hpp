#pragma once
#include <vector>
#include <SDL2/SDL.h>

typedef struct {
    float x;
    float y;
    float z;
} Vector3;

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    float u;
    float v;
} UV;

typedef struct {
    Vector3 vertexes[3];
    SDL_Point vertexesProjected[3];
    Vector3 vectorNormals[3];
    UV uv[3];
    Vector3 faceNormal;
    Uint8 color[3];
} Face3D;

typedef struct {
    std::vector<Face3D> faces;
    std::vector<Uint32> texture;
    int texWidth;
    int texHeight;
    Vector3 position;
    Vector3 rotation;
} Object3D;