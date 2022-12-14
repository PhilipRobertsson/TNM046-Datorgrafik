/*
 * A class to manage a basic OpenGL VertexArray and index array in an OpenGL VertexArray object.
 *
 * Usage: The methods createXXX() create geometry from fixed arrays or procedural
 *        descriptions.
 *        The method loadOBJ() loads geometry from an OBJ file. Only the mesh is loaded. Material
 *        information is ignored. Only triangles are supported. OBJ files with quads are rejected.
 *        Call render() to draw the mesh in OpenGL.
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2013-2014
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#pragma once

#include <GLFW/glfw3.h>  // To use OpenGL datatypes
#include <string>
#include <vector>

// A class to hold geometry data and send it off for rendering
class TriangleSoup {
public:
    /* Constructor: initialize a triangleSoup object to all zeros */
    TriangleSoup();

    /* Destructor: clean up allocated data in a triangleSoup object */
    ~TriangleSoup();

    /* Clean up allocated data in a triangleSoup object */
    void clean();

    /* Create a very simple demo mesh with a single triangle */
    void createTriangle();

    /* Create a simple box geometry */
    void createBox(float xsize, float ysize, float zsize);

    /* Create a sphere (approximated by polygon segments) */
    void createSphere(float radius, int segments);

    /* Load geometry from an OBJ file */
    void readOBJ(const std::string& filename);

    /* Print data from a triangleSoup object, for debugging purposes */
    void print();

    /* Print information about a triangleSoup object (stats and extents) */
    void printInfo();

    /* Render the geometry in a triangleSoup object */
    void render();

private:
    void printError(const char* errtype, const char* errmsg);

    GLuint vao_;                        // Vertex array object, the main handle for geometry
    int nverts_;                        // Number of vertices in the vertex array
    int ntris_;                         // Number of triangles in the index array (may be zero)
    GLuint vertexbuffer_;               // Buffer ID to bind to GL_ARRAY_BUFFER
    GLuint indexbuffer_;                // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
    std::vector<GLfloat> vertexarray_;  // Vertex array on interleaved format: x y z nx ny nz s t
    std::vector<GLuint> indexarray_;    // Element index array
};
