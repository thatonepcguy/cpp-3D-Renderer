#include <iostream>
#include <vector>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include "objParser.hpp"
#include "main.hpp"
#include "objectRasterizer.hpp"

#define WIDTH 1280
#define HEIGHT 720
#define FOV 500

void rotationDotProduct(float v[3], float rotMatrix[3][3]) {
    float rotV[3];
    rotV[0] = (rotMatrix[0][0] * v[0]) + (rotMatrix[0][1] * v[1]) + (rotMatrix[0][2] * v[2]);
    rotV[1] = (rotMatrix[1][0] * v[0]) + (rotMatrix[1][1] * v[1]) + (rotMatrix[1][2] * v[2]);
    rotV[2] = (rotMatrix[2][0] * v[0]) + (rotMatrix[2][1] * v[1]) + (rotMatrix[2][2] * v[2]);
    v[0] = rotV[0];
    v[1] = rotV[1];
    v[2] = rotV[2];
}

float calculateFaceDistance(const Face3D& face) {
    float distanceSum = 0.0f;
    for (int i = 0; i < 3; ++i) {
        const auto& vertex = face.vertexes[i];
        float distance = std::sqrt(vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z);
        distanceSum += distance;
    }
    return distanceSum / 3.0f;
}

bool compareFacesByDistance(const Face3D& a, const Face3D& b) {
    return calculateFaceDistance(a) > calculateFaceDistance(b); // Sort in descending order
}

Object3D zOrder(const Object3D& object) {
    Object3D objectOrdered = object;

    // Sort faces by distance
    std::sort(objectOrdered.faces.begin(), objectOrdered.faces.end(), compareFacesByDistance);

    return objectOrdered;
}

Object3D behindCulling(Object3D object) {
    Object3D objectCulled = object;
    for (int face = 0; face < objectCulled.faces.size(); face++) {
        int vertexesBehind = 0;
        for (int vertex = 0; vertex < 3; vertex++) {
            if (objectCulled.faces[face].vertexes[vertex].z > 0) {
                vertexesBehind++;
            }
        }
        if (vertexesBehind == 3) {
            objectCulled.faces.erase(objectCulled.faces.begin() + face);
            face--;
        } else if (vertexesBehind > 0 && vertexesBehind < 3) {
            for (int vertex = 0; vertex < 3; vertex++) {
                if (objectCulled.faces[face].vertexes[vertex].z > 0) {
                    objectCulled.faces[face].vertexes[vertex].z = 0;
                }
            }
        }
    }
    return objectCulled;
}

void render(Object3D object, SDL_Renderer *renderer) {
    rasterizeObject(object, renderer);
}

Object3D project(Object3D object, int fov) {
    Object3D objectProjected = object;
    int Fov = fov;

    for (int face = 0; face < object.faces.size(); face++) {
        for (int vertex = 0; vertex < 3; vertex++) {
            objectProjected.faces[face].vertexesProjected[vertex].x = (objectProjected.faces[face].vertexes[vertex].x * Fov/ objectProjected.faces[face].vertexes[vertex].z)+ WIDTH / 2;
            objectProjected.faces[face].vertexesProjected[vertex].y = (objectProjected.faces[face].vertexes[vertex].y * Fov / objectProjected.faces[face].vertexes[vertex].z) + HEIGHT / 2;

        }
    }

    return objectProjected;
}

Object3D transform(Object3D object, Vector3 cameraPos) {
    Object3D objectTransformed = object;

    for (int face = 0; face < object.faces.size(); face++) {
        for (int vertex = 0; vertex < 3; vertex++) {
            Vector3 v = {object.faces[face].vertexes[vertex].x, object.faces[face].vertexes[vertex].y, object.faces[face].vertexes[vertex].z};
            v.x -= cameraPos.x;
            v.y -= cameraPos.y;
            v.z -= cameraPos.z;
            objectTransformed.faces[face].vertexes[vertex].x = v.x;
            objectTransformed.faces[face].vertexes[vertex].y = v.y;
            objectTransformed.faces[face].vertexes[vertex].z = v.z;
        }
    }

    return objectTransformed;
}

Object3D rotate(Object3D object, Vector3 cameraRot) {
    Object3D objectRotated = object;

    float rotMatrixY[3][3] = {{cos(cameraRot.y), 0, sin(cameraRot.y)},
                              {0, 1, 0},
                              {-sin(cameraRot.y), 0, cos(cameraRot.y)}};
    float rotMatrixX[3][3] = {{1, 0, 0},
                              {0, cos(cameraRot.x), -sin(cameraRot.x)},
                              {0, sin(cameraRot.x), cos(cameraRot.x)}};

    for (int face = 0; face < object.faces.size(); face++) {
        for (int vertex = 0; vertex < 3; vertex++) {
            float v[3] = {object.faces[face].vertexes[vertex].x, object.faces[face].vertexes[vertex].y, object.faces[face].vertexes[vertex].z};
            rotationDotProduct(v, rotMatrixY);
            rotationDotProduct(v, rotMatrixX);

            objectRotated.faces[face].vertexes[vertex].x = v[0];
            objectRotated.faces[face].vertexes[vertex].y = v[1];
            objectRotated.faces[face].vertexes[vertex].z = v[2];
            
        }
    }

    return objectRotated;
}

int main() {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    int SDL_FLAGS = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    window = SDL_CreateWindow("3D-Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_SWSURFACE);
    renderer = SDL_CreateRenderer(window, -1, SDL_FLAGS);

    // Create Object
    std::string path = "lander_A.obj";
    Object3D object = parse(path);

    // Camera
    Vector3 cameraPos = {0, 0, 10};
    Vector3 cameraRot = {0, 0, 0};

    // MAIN LOOP
    bool QUIT = false;
    std::string keyPressed = "";
    
    while (!QUIT) {
        // Handle Events
        SDL_Event event;
        while (SDL_PollEvent(&event) != 0){
            switch (event.type) {
                case SDL_QUIT:
                    QUIT = true;  
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_a:
                            keyPressed = "a";
                            break;
                        case SDLK_d:
                            keyPressed = "d";
                            break;
                        case SDLK_w:
                            keyPressed = "w";
                            break;
                        case SDLK_s:
                            keyPressed = "s";
                            break;
                        case SDLK_SPACE:
                            keyPressed = "sp";
                            break;
                        case SDLK_LSHIFT:
                            keyPressed = "sh";
                            break;
                        case SDLK_RIGHT:
                            keyPressed = "kr";
                            break;
                        case SDLK_LEFT:
                            keyPressed = "kl";
                            break;
                        case SDLK_UP:
                            keyPressed = "ku";
                            break;
                        case SDLK_DOWN:
                            keyPressed = "kd";
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                        case SDLK_a:
                        case SDLK_d:
                        case SDLK_w:
                        case SDLK_s:
                        case SDLK_SPACE:
                        case SDLK_LSHIFT:
                        case SDLK_RIGHT:
                        case SDLK_LEFT:
                        case SDLK_UP:
                        case SDLK_DOWN:
                            keyPressed = "";
                            break;
                    }
                    break;

                default:
                    break;
            }
        }
        // input
        if (keyPressed == "w") {
            cameraPos.x += 1*sin(cameraRot.y);
            cameraPos.z -= 1*cos(cameraRot.y);
        } else if (keyPressed == "s") {
            cameraPos.x -= 1*sin(cameraRot.y);
            cameraPos.z += 1*cos(cameraRot.y);
        } else if (keyPressed == "d") {
            cameraPos.x -= 1*cos(cameraRot.y);
            cameraPos.z -= 1*sin(cameraRot.y);
        } else if (keyPressed == "a") {
            cameraPos.x += 1*cos(cameraRot.y);
            cameraPos.z += 1*sin(cameraRot.y);
        } else if (keyPressed == "sp") {
            cameraPos.y += 1;
        } else if (keyPressed == "sh") {
            cameraPos.y -= 1;
        } else if (keyPressed == "kr") {
            cameraRot.y -= 0.05236;
        } else if (keyPressed == "kl") {
            cameraRot.y += 0.05236;
        } else if (keyPressed == "ku") {
            cameraRot.x -= 0.05236;
        } else if (keyPressed == "kd") {
            cameraRot.x += 0.05236;
        }

        Object3D object3D = object;
        // perform Object3D transformations
        object3D = transform(object3D, cameraPos);
        object3D = rotate(object3D, cameraRot);
        object3D = behindCulling(object3D);
        object3D = zOrder(object3D);
        object3D = project(object3D, FOV);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        render(object3D, renderer);
        SDL_RenderPresent(renderer);

        // sleep ~60fps
        usleep(16666);
    }

    // QUIT
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    SDL_Quit();

    return 0;
}