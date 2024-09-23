#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <cmath>
#include <algorithm>
#include "objectRasterizer.hpp"
#include "main.hpp"

std::vector<Vector2Int> plotLine(int x0, int y0, int x1, int y1, SDL_Renderer *renderer) {
    std::vector<Vector2Int> points;
    // Bresenham's line drawing algorithim 
    int dx = abs(x1-x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int error = dx+dy;
    while (true) {
        points.push_back({x0, y0});  
        if (x0 == x1 && y0 == y1) {break;}
        if (x0 > 640 || x0 < 0 || y0 > 360 || y0 < 0) {break;}
        int e2 = 2 * error;
        if (e2 >= dy) {
            error = error + dy;
            x0 = x0 +sx;
        }
        if (e2 <= dx) {
            error = error + dx;
            y0 = y0 + sy;
        }
    }

    return points;
}

void fillTriangle(std::vector<Vector2Int> side1, std::vector<Vector2Int> side2, std::vector<Vector2Int> side3, SDL_Renderer* renderer) {
    std::vector<Vector2Int> sides = side1;
    sides.insert(sides.end(), side2.begin(), side2.end());
    sides.insert(sides.end(), side3.begin(), side3.end());

    int minY = 360;
    int maxY = 0;
    for (int point = 0; point < sides.size(); point++) {
        if (sides[point].y > maxY) {
            maxY = sides[point].y;
        }
        if (sides[point].y < minY) {
            minY = sides[point].y;
        }
    }

    int dy = maxY-minY;
    std::vector<Vector2Int> points;

    for (int y  = 0; y < dy+1; y++) {
        for (int point = 0; point < sides.size(); point++) {
            if (sides[point].y == minY+y) {
                points.push_back(sides[point]);
            }
        }
        int minX = 640;
        int maxX = 0;
        for (int point = 0; point < points.size(); point++) {
            if (points[point].x > maxX) {
                maxX = points[point].x;
            }
            if (points[point].x < minX) {
                minX = points[point].x;
            }
        }
        if (maxX < minX) {
            maxX = minX;
        }
        SDL_RenderDrawLine(renderer, minX, minY+y, maxX, minY+y);
        points.clear();
        
    }
}



void rasterizeObject(Object3D object, SDL_Renderer* renderer) {
    // check for completely off screen points
    for (int face = 0; face < object.faces.size(); face++) {
        int vertexesOffscreen = 0;
        for (int vertex = 0; vertex < 3; vertex++) {
            if (object.faces[face].vertexesProjected[vertex].x < 0 || object.faces[face].vertexesProjected[vertex].x > 640 || object.faces[face].vertexesProjected[vertex].y < 0 || object.faces[face].vertexesProjected[vertex].y > 360) {
                object.faces[face].vertexesProjected[vertex].x = std::clamp(object.faces[face].vertexesProjected[vertex].x, 0, 640);
                object.faces[face].vertexesProjected[vertex].y = std::clamp(object.faces[face].vertexesProjected[vertex].y, 0, 360);
                vertexesOffscreen++;
            }
        }
        if (vertexesOffscreen < 3) {
            SDL_SetRenderDrawColor(renderer, object.faces[face].color[0], object.faces[face].color[1], object.faces[face].color[2], 255);
            std::vector<Vector2Int> side1 = plotLine(static_cast<int>(std::round(object.faces[face].vertexesProjected[0].x)), static_cast<int>(std::round(object.faces[face].vertexesProjected[0].y)), static_cast<int>(std::round(object.faces[face].vertexesProjected[1].x)), static_cast<int>(std::round(object.faces[face].vertexesProjected[1].y)), renderer);
            std::vector<Vector2Int> side2 = plotLine(static_cast<int>(std::round(object.faces[face].vertexesProjected[1].x)), static_cast<int>(std::round(object.faces[face].vertexesProjected[1].y)), static_cast<int>(std::round(object.faces[face].vertexesProjected[2].x)), static_cast<int>(std::round(object.faces[face].vertexesProjected[2].y)), renderer);
            std::vector<Vector2Int> side3 = plotLine(static_cast<int>(std::round(object.faces[face].vertexesProjected[2].x)), static_cast<int>(std::round(object.faces[face].vertexesProjected[2].y)), static_cast<int>(std::round(object.faces[face].vertexesProjected[0].x)), static_cast<int>(std::round(object.faces[face].vertexesProjected[0].y)), renderer);
            fillTriangle(side1, side2, side3, renderer);
        }
    }
}
