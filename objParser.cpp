#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "objParser.hpp"
#include "main.hpp"

std::vector<std::string> tokenize(const std::string& s, const std::string& del = " ") {
    std::vector<std::string> tokens;
    int start = 0;
    int end = s.find(del);

    while (end != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + del.size();
        end = s.find(del, start);
    }

    tokens.push_back(s.substr(start));
    return tokens;
}


Object3D parse(std::string path) {
    std::string line;
    std::ifstream FILE(path, std::ios::in);

    std::vector<std::vector<float>> vertexes; 
    std::vector<std::vector<float>> normals; 
    std::vector<Face3D> faces;

    Object3D object;
    while (std::getline(FILE, line)) {
        if (line.rfind("v ", 0) == 0) {
            std::vector<std::string> parts = tokenize(line, " ");
            vertexes.push_back({std::stof(parts[1]),std::stof(parts[2]), std::stof(parts[3])});
        }
        if (line.rfind("vn ", 0) == 0) {
            std::vector<std::string> parts = tokenize(line, " ");
            normals.push_back({std::stof(parts[1]),std::stof(parts[2]), std::stof(parts[3])});
        }
        if (line.rfind("f ", 0) == 0) {
            std::vector<std::string> parts = tokenize(line, " ");
            std::vector<std::string> v1Parts = tokenize(parts[1], "/");
            std::vector<std::string> v2Parts = tokenize(parts[2], "/");
            std::vector<std::string> v3Parts = tokenize(parts[3], "/");
            std::string v1 = v1Parts[0];
            std::string v2 = v2Parts[0];
            std::string v3 = v3Parts[0];
            std::string n1 = v1Parts[2];
            std::string n2 = v2Parts[2];
            std::string n3 = v3Parts[2];
            Face3D face;
            face.vertexes[0] = {vertexes[std::stoi(v1)-1][0], vertexes[std::stoi(v1)-1][1], vertexes[std::stoi(v1)-1][2]};
            face.vertexes[1] = {vertexes[std::stoi(v2)-1][0], vertexes[std::stoi(v2)-1][1], vertexes[std::stoi(v2)-1][2]};
            face.vertexes[2] = {vertexes[std::stoi(v3)-1][0], vertexes[std::stoi(v3)-1][1], vertexes[std::stoi(v3)-1][2]};
            face.vectorNormals[0] = {normals[std::stoi(v1)-1][0], normals[std::stoi(v1)-1][1], normals[std::stoi(v1)-1][2]};
            face.vectorNormals[1] = {normals[std::stoi(v2)-1][0], normals[std::stoi(v2)-1][1], normals[std::stoi(v2)-1][2]};
            face.vectorNormals[2] = {normals[std::stoi(v3)-1][0], normals[std::stoi(v3)-1][1], normals[std::stoi(v3)-1][2]};

            face.faceNormal.x = (face.vectorNormals[0].x+face.vectorNormals[1].x+face.vectorNormals[2].x) / 3;
            face.faceNormal.y = (face.vectorNormals[0].y+face.vectorNormals[1].y+face.vectorNormals[2].y) / 3;
            face.faceNormal.z = (face.vectorNormals[0].z+face.vectorNormals[1].z+face.vectorNormals[2].z) / 3;
            
            face.color[0] = static_cast<Uint8>(std::round((face.faceNormal.x + 1) * 127.5));
            face.color[1] = static_cast<Uint8>(std::round((face.faceNormal.y + 1) * 127.5));
            face.color[2] = static_cast<Uint8>(std::round((face.faceNormal.z + 1) * 127.5));
            faces.push_back(face);
        }
        object.faces = faces;
    }


    FILE.close();
    return object;
}