#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "objParser.hpp"
#include "main.hpp"

int find(std::vector<std::string> arr, std::string seek)
{
    for (int i = 0; i < arr.size(); ++i)
    {
        if (arr[i] == seek) { return i;}
    }
    return -1;
}

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


Object3D parse(std::string path, std::string mtlPath) {
    std::string MTLline;
    std::ifstream MTLFILE(mtlPath, std::ios::in);

    std::vector<std::string> colorNames;
    std::vector<std::vector<Uint8>> colors;

    while (std::getline(MTLFILE, MTLline)) {
        if (MTLline.rfind("newmtl ", 0) == 0) {
            std::vector<std::string> parts = tokenize(MTLline, " ");
            colorNames.push_back(parts[1]);
        } 
        if (MTLline.rfind("Kd ", 0) == 0) {
            std::vector<std::string> parts = tokenize(MTLline, " ");
            colors.push_back({static_cast<Uint8>(round(std::stof(parts[1])*255)), 
                               static_cast<Uint8>(round(std::stof(parts[2])*255)),
                               static_cast<Uint8>(round(std::stof(parts[3])*255))});
        }
    }

    MTLFILE.close();

    std::vector<std::vector<float>> vertexes; 
    std::vector<std::vector<float>> normals; 
    std::vector<Face3D> faces;

    Uint8 currentColor[3];
    std::string VERTline;

    std::ifstream FILE(path, std::ios::in);

    Object3D object;
    while (std::getline(FILE, VERTline)) {
        if (VERTline.rfind("v ", 0) == 0) {
            std::vector<std::string> parts = tokenize(VERTline, " ");
            vertexes.push_back({std::stof(parts[1]),std::stof(parts[2]), std::stof(parts[3])});
        }
        if (VERTline.rfind("vn ", 0) == 0) {
            std::vector<std::string> parts = tokenize(VERTline, " ");
            normals.push_back({std::stof(parts[1]),std::stof(parts[2]), std::stof(parts[3])});
        }
    }
    FILE.close();

    std::cout << "V DONE" << "\n";

    std::ifstream FACEFILE(path, std::ios::in);

    std::string line;
    while (std::getline(FACEFILE, line)) {
        if (line.rfind("usemtl ", 0) == 0) {
            std::vector<std::string> parts = tokenize(line, " ");
            int index = find(colorNames, parts[1]);
            currentColor[0] = colors.at(index)[0];
            currentColor[1] = colors.at(index)[1];
            currentColor[2] = colors.at(index)[2];
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

            face.vectorNormals[0] = {normals[std::stoi(n1)-1][0], normals[std::stoi(n1)-1][1], normals[std::stoi(n1)-1][2]};
            face.vectorNormals[1] = {normals[std::stoi(n2)-1][0], normals[std::stoi(n2)-1][1], normals[std::stoi(n2)-1][2]};
            face.vectorNormals[2] = {normals[std::stoi(n3)-1][0], normals[std::stoi(n3)-1][1], normals[std::stoi(n3)-1][2]};

            face.faceNormal.x = (face.vectorNormals[0].x+face.vectorNormals[1].x+face.vectorNormals[2].x) / 3;
            face.faceNormal.y = (face.vectorNormals[0].y+face.vectorNormals[1].y+face.vectorNormals[2].y) / 3;
            face.faceNormal.z = (face.vectorNormals[0].z+face.vectorNormals[1].z+face.vectorNormals[2].z) / 3;

            float magnitude = sqrt(face.faceNormal.x*face.faceNormal.x + face.faceNormal.y*face.faceNormal.y + face.faceNormal.z*face.faceNormal.z);
            face.faceNormal.x = face.faceNormal.x / magnitude;
            face.faceNormal.y = face.faceNormal.y / magnitude;
            face.faceNormal.z = face.faceNormal.z / magnitude;
            
            face.color[0] = currentColor[0];
            face.color[1] = currentColor[1];
            face.color[2] = currentColor[2];
            faces.push_back(face);
        }
        object.faces = faces;
    }


    FACEFILE.close();
    return object;
}