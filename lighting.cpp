#include "lighting.hpp"
#include "main.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

// FAST INVERSE SQRT CAUSE CPUS ARE SLOW 
float fastInvSqrt(float x){
    float xhalf = 0.5f * x;
    int i = *(int*)&x;            // store floating-point bits in integer
    i = 0x5f3759df - (i >> 1);    // initial guess for Newton's method
    x = *(float*)&i;              // convert new bits into float
    x = x*(1.5f - xhalf*x*x);     // One round of Newton's method
    return x;
}

Object3D calculateLighting(Object3D object, Vector3 lightDir) {
    Object3D objectLighted = object;

    // convert camera angles to vector
    Vector3 light = {cos(lightDir.x), cos(lightDir.y), cos(lightDir.z)};
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

        angle /= 3.1415926/2;
        int color[3] = {std::clamp(static_cast<int>(roundeven(angle*object.faces[face].color[0])), 0, 255), std::clamp(static_cast<int>(roundeven(angle*object.faces[face].color[1])),0,255), std::clamp(static_cast<int>(roundeven(angle*object.faces[face].color[2])),0, 255)};
        objectLighted.faces[face].color[0] = static_cast<Uint8>(color[0]);
        objectLighted.faces[face].color[1] = static_cast<Uint8>(color[1]);
        objectLighted.faces[face].color[2] = static_cast<Uint8>(color[2]);
    }
    

    return objectLighted;
}