#pragma once
#include<glad/glad.h>
#include<Primitives/Vertex.h>

class Vertex;

// All ptrs are of 4bytes. Also ptrs are used so that we dont copy data
struct ShapeData
{
    ShapeData() : vertices(NULL), numVertices(0), indices(NULL), numIndices(0) {}

    Vertex* vertices;
    GLuint numVertices;
    GLushort* indices;
    GLuint numIndices;

    GLsizeiptr vertexBufferSize() const {
        return numVertices * sizeof(Vertex); // can do return sizeof(vertices) directly, but since its a reference, we better not mess with it       
    }

    GLsizeiptr indexBufferSize() const {
        return numIndices * sizeof(GLushort);
    }

    /*
    ~ShapeData() { // This may sometimes lead to deleting the data after ShapeGenerator::makeTriangle() finishes, and will return empty data to the main calling fn, so manual cleanup recommended
        delete [] vertices;
        delete [] indices;
        numVertices = numIndices = 0;
    } // just deep and shallow copy stuff. TODO: research about it
    */

    void cleanup() {
        delete [] vertices;
        delete [] indices;
        numVertices = numIndices = 0;
    }
};
