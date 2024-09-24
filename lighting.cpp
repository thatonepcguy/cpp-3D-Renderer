#include "lighting.hpp"
#include "main.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include "shared.hpp"

Object3D calculateLighting(Object3D object, Vector3 lightDir) {
    Object3D objectLighted = object;

    // convert light angles to vector
    Vector3 forward = {0,0,1};
    float Y_MATRIX[3][3] = {{cos(lightDir.y), 0, sin(lightDir.y)},
                            {0, 1, 0},
                            {-sin(lightDir.y), 0, cos(lightDir.y)}};
    float X_MATRIX[3][3] = {{1, 0,0},
                            {0, cos(lightDir.x), -sin(lightDir.x)},
                            {0, sin(lightDir.x), cos(lightDir.x)}};

    Vector3 lightYDir = {(forward.x*Y_MATRIX[0][0])+(forward.y*Y_MATRIX[0][1])+(forward.z*Y_MATRIX[0][2]),
                          (forward.x*Y_MATRIX[1][0])+(forward.y*Y_MATRIX[1][1])+(forward.z*Y_MATRIX[1][2]),
                          (forward.x*Y_MATRIX[2][0])+(forward.y*Y_MATRIX[2][1])+(forward.z*Y_MATRIX[2][2])};
    Vector3 lightXDir = {(lightYDir.x*X_MATRIX[0][0])+(lightYDir.y*X_MATRIX[0][1])+(lightYDir.z*X_MATRIX[0][2]),
                          (lightYDir.x*X_MATRIX[1][0])+(lightYDir.y*X_MATRIX[1][1])+(lightYDir.z*X_MATRIX[1][2]),
                          (lightYDir.x*X_MATRIX[2][0])+(lightYDir.y*X_MATRIX[2][1])+(lightYDir.z*X_MATRIX[2][2])};

    Vector3 light = {lightXDir.x, lightXDir.y, lightXDir.z};

    float inverseSqrtLight = fastInvSqrt(light.x*light.x + light.y*light.y + light.z*light.z);
    light.x *= inverseSqrtLight;
    light.y *= inverseSqrtLight;
    light.z *= inverseSqrtLight;
    
    for (int face = 0; face < object.faces.size(); face++) {
        object.faces[face].faceNormal;
        // im not 100% sure i have to do this but just in case
        float inverseSqrtFace = fastInvSqrt(object.faces[face].faceNormal.x*object.faces[face].faceNormal.x + object.faces[face].faceNormal.y*object.faces[face].faceNormal.y + object.faces[face].faceNormal.z*object.faces[face].faceNormal.z);
        Vector3 faceNormal = {object.faces[face].faceNormal.x*inverseSqrtFace, object.faces[face].faceNormal.y*inverseSqrtFace, object.faces[face].faceNormal.z*inverseSqrtFace};
        
        // angle
        float dotProduct  = std::clamp(light.x*object.faces[face].faceNormal.x + light.y*object.faces[face].faceNormal.y + light.z*object.faces[face].faceNormal.z, -1.0f, 1.0f);
        float angle = acos(dotProduct);

        angle /= 3.1415926;
        int color[3] = {std::clamp(static_cast<int>(roundeven(angle*object.faces[face].color[0])), 0, 255), std::clamp(static_cast<int>(roundeven(angle*object.faces[face].color[1])),0,255), std::clamp(static_cast<int>(roundeven(angle*object.faces[face].color[2])),0, 255)};
        objectLighted.faces[face].color[0] = static_cast<Uint8>(color[0]);
        objectLighted.faces[face].color[1] = static_cast<Uint8>(color[1]);
        objectLighted.faces[face].color[2] = static_cast<Uint8>(color[2]);
    }
    

    return objectLighted;
}