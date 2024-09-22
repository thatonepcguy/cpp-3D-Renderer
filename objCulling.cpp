#include "main.hpp"
#include "objCulling.hpp"


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
