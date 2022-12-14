/*
 * OpenGL VertexArray handling
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2014
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#if defined(WIN32) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <GL/glew.h>

#include <cstdio>
#include <iostream>
#include <algorithm>

#include "TriangleSoup.hpp"

/* Constructor: initialize a TriangleSoup object to an empty object */
TriangleSoup::TriangleSoup() : vao_(0), vertexbuffer_(0), indexbuffer_(0), nverts_(0), ntris_(0) {}

/* Destructor: clean up allocated data in a TriangleSoup object */
TriangleSoup::~TriangleSoup() { clean(); }

/* Clean up, remembering to de-allocate arrays and GL resources */
void TriangleSoup::clean() {
    if (glIsVertexArray(vao_)) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    if (glIsBuffer(vertexbuffer_)) {
        glDeleteBuffers(1, &vertexbuffer_);
        vertexbuffer_ = 0;
    }

    if (glIsBuffer(indexbuffer_)) {
        glDeleteBuffers(1, &indexbuffer_);
        indexbuffer_ = 0;
    }

    vertexarray_.clear();
    indexarray_.clear();
    nverts_ = 0;
    ntris_ = 0;
}

/* Create a demo object with a single triangle */
void TriangleSoup::createTriangle() {

    // Constant data arrays for this simple test.
    // Note, however, that they are copied to dynamic arrays
    // in the class, to handle this object in the same manner
    // as the larger objects loaded from file.
    //
    // The data array contains 8 floats per vertex:
    // coordinate xyz, normal xyz, texcoords st
    const GLfloat vertex_array_data[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // Vertex 0
        1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // Vertex 1
        0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f   // Vertex 2
    };
    const GLuint index_array_data[] = {0, 1, 2};

    nverts_ = 3;
    ntris_ = 1;

    vertexarray_.resize(8 * nverts_);
    indexarray_.resize(3 * ntris_);

    for (int i = 0; i < 8 * nverts_; i++) {
        vertexarray_[i] = vertex_array_data[i];
    }
    for (int i = 0; i < 3 * ntris_; i++) {
        indexarray_[i] = index_array_data[i];
    }

    // Generate one vertex array object (VAO) and bind it
    glGenVertexArrays(1, &(vao_));
    glBindVertexArray(vao_);

    // Generate two buffer IDs
    glGenBuffers(1, &vertexbuffer_);
    glGenBuffers(1, &indexbuffer_);

    // Activate the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_);
    // Present our vertex coordinates to OpenGL (8 * nverts_)
    glBufferData(GL_ARRAY_BUFFER, vertexarray_.size() * sizeof(GLfloat), vertexarray_.data(),
                 GL_STATIC_DRAW);
    // Specify how many attribute arrays we have in our VAO
    glEnableVertexAttribArray(0);  // Vertex coordinates
    glEnableVertexAttribArray(1);  // Normals
    glEnableVertexAttribArray(2);  // Texture coordinates
    // Specify how OpenGL should interpret the vertex buffer data:
    // Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
    // Number of dimensions (3 means vec3 in the shader, 2 means vec2)
    // Type GL_FLOAT
    // Not normalized (GL_FALSE)
    // Stride 8 floats (interleaved array with 8 floats per vertex)
    // Array buffer offset 0, 3 or 6 floats (offset into first vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)0);  // xyz coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));  // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(6 * sizeof(GLfloat)));  // texcoords

    // Activate the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer_);
    // Present our vertex indices to OpenGL (3 * ntris_)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexarray_.size() * sizeof(GLuint), indexarray_.data(),
                 GL_STATIC_DRAW);

    // Deactivate (unbind) the VAO and the buffers again.
    // Do NOT unbind the index buffer while the VAO is still bound.
    // The index buffer is an essential part of the VAO state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/* Create a simple box geometry */
/* TODO: Split to 24 vertices to get the normals and texcoords right. */
void TriangleSoup::createBox(float xsize, float ysize, float zsize) {

    // The data array contains 8 floats per vertex:
    // coordinate xyz, normal xyz, texcoords st
    const GLfloat vertex_array_data[] = {
        // side 1 höger normal (1.0f, 0.0f, 0.0f) done
        // side 2 top normal (0.0f, 1.0f, 0.0f) done
        // side 3 fram normal (0.0f, 0.0f, 1.0f) done
        // side 4 ner normal (0.0f, -1.0f, 0.0f) done
        // side 5 bak normal (0.0f, 0.0f, -1.0f) done
        // side 6 vänster normal (-1.0f, 0.0f, 0.0f) done
        xsize,  -ysize, -zsize, 1.0f, 0.0f,   0.0f,   0.0f,   1.0f,  // 0.1     0   taken   side 1 p4 done
        xsize,  -ysize, -zsize, 0.0f,-1.0f,  0.0f,   1.0f,   1.0f,  // 0.2     1   taken   side 4 p4 done
        xsize,  -ysize, -zsize, 0.0f, 0.0f,   -1.0f,  1.0f,   0.0f,  // 0.3      2   taken   side 5 p4 done
        xsize,  ysize,  -zsize, 1.0f, 0.0f,   0.0f,   1.0f,   1.0f,  // 1.1      3   taken   side 1 p7 done
        xsize,  ysize,  -zsize, 0.0f, 1.0f,   0.0f,   1.0f,   1.0f,  // 1.2    4   taken   side 2 p7 done
        xsize,  ysize,  -zsize, 0.0f, 0.0f,   -1.0f,  1.0f,   1.0f,  // 1.3    5   taken   side 5 p7 done
        xsize,  ysize,  zsize,  1.0f, 0.0f,   0.0f,   1.0f,   0.0f,  // 2.1       6   taken   side 1 p6 done
        xsize,  ysize,  zsize,  0.0f, 1.0f,   0.0f,   1.0f,   0.0f,  // 2.2    7   taken   side 2 p6 done
        xsize,  ysize,  zsize,  0.0f, 0.0f,   1.0f,   1.0f,   1.0f,  // 2.3    8   taken   side 3 p6 done
        xsize,  -ysize, zsize,  1.0f, 0.0f,   0.0f,   0.0f,   0.0f,  // 3.1     9   taken   side 1 p5 done
        xsize,  -ysize, zsize,  0.0f, 0.0f,   1.0f,   1.0f,   0.0f,  // 3.2    10  taken   side 3 p5 done
        xsize,  -ysize, zsize,  0.0f, 1.0f,  0.0f,   1.0f,   0.0f,  // 3.3    11  taken   side 4 p5 done

        -xsize, -ysize, -zsize, 0.0f, -1.0f,  0.0f,   0.0f,   1.0f,  // 4.1  12  taken   side 4 p0 done
        -xsize, -ysize, -zsize, 0.0f, 0.0f,   -1.0f,  0.0f,   0.0f,  // 4.2    13  taken   side 5 p0 done
        -xsize, -ysize, -zsize, -1.0f, 0.0f,   0.0f,   0.0f,   0.0f,  // 4.3    14  taken   side 6 p0  done
        -xsize, -ysize, zsize,  0.0f, 0.0f,   1.0f,   0.0f,   0.0f,  // 5.1    15  taken   side 3 p1 done
        -xsize, -ysize, zsize,  0.0f, -1.0f,  0.0f,   0.0f,   0.0f,  // 5.2     16  taken   side 4 p1 done
        -xsize, -ysize, zsize,  -1.0f, 0.0f,   0.0f,   1.0f,   0.0f,  // 5.3    17  taken   side 6 p1 done
        -xsize, ysize,  -zsize, 0.0f, 1.0f,   0.0f,   0.0f,   1.0f,  // 6.1     18  taken   side 2 p3 done
        -xsize, ysize,  -zsize, 0.0f, 0.0f,   -1.0f,  0.0f,   1.0f,  // 6.2    19  taken   side 5 p3 done
        -xsize, ysize,  -zsize, -1.0f, 0.0f,   0.0f,   0.0f,   1.0f,  // 6.3    20  taken   side 6 p3 done
        -xsize, ysize,  zsize,  0.0f, 1.0f,   0.0f,   0.0f,   0.0f,  // 7.1       21  taken   side 2 p2 done
        -xsize, ysize,  zsize,  0.0f, 0.0f,   1.0f,   0.0f,   1.0f,  // 7.2    22  taken   side 3 p2 done
        -xsize, ysize,  zsize,  -1.0f, 0.0f,   0.0f,   1.0f,   1.0f,  // 7.3    23  taken   side 6 p2 done

    };
    const GLuint index_array_data[] = {
        0,  3,  9,  3,  6,  9,   // side 1   -1.0f, 0.0f, 0.0f klar yes
        4,  18, 21, 4,  21, 7,   // side 2
        10, 8,  22, 10, 22, 15,  // side 3
        1,  11, 16, 1,  16, 12,  // side 4
        2,  19, 5,  2,  13, 19,  // side 5
        20, 14, 17, 20, 17, 23,  // side 6
    };

    nverts_ = 24;
    ntris_ = 12;

    vertexarray_.resize(8 * nverts_);
    indexarray_.resize(3 * ntris_);

    for (int i = 0; i < 8 * nverts_; i++) {
        vertexarray_[i] = vertex_array_data[i];
    }
    for (int i = 0; i < 3 * ntris_; i++) {
        indexarray_[i] = index_array_data[i];
    }

    // Generate one vertex array object (VAO) and bind it
    glGenVertexArrays(1, &(vao_));
    glBindVertexArray(vao_);

    // Generate two buffer IDs
    glGenBuffers(1, &vertexbuffer_);
    glGenBuffers(1, &indexbuffer_);

    // Activate the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_);
    // Present our vertex coordinates to OpenGL (8 * nverts_)
    glBufferData(GL_ARRAY_BUFFER, vertexarray_.size() * sizeof(GLfloat), vertexarray_.data(),
                 GL_STATIC_DRAW);
    // Specify how many attribute arrays we have in our VAO
    glEnableVertexAttribArray(0);  // Vertex coordinates
    glEnableVertexAttribArray(1);  // Normals
    glEnableVertexAttribArray(2);  // Texture coordinates
    // Specify how OpenGL should interpret the vertex buffer data:
    // Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
    // Number of dimensions (3 means vec3 in the shader, 2 means vec2)
    // Type GL_FLOAT
    // Not normalized (GL_FALSE)
    // Stride 8 floats (interleaved array with 8 floats per vertex)
    // Array buffer offset 0, 3 or 6 floats (offset into first vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)0);  // xyz coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));  // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(6 * sizeof(GLfloat)));  // texcoords

    // Activate the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer_);
    // Present our vertex indices to OpenGL (3 * ntris_)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexarray_.size() * sizeof(GLuint), indexarray_.data(),
                 GL_STATIC_DRAW);

    // Deactivate (unbind) the VAO and the buffers again.
    // Do NOT unbind the index buffer while the VAO is still bound.
    // The index buffer is an essential part of the VAO state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void TriangleSoup::createSphere(float radius, int segments) {
    // Delete any previous content in the TriangleSoup object
    clean();

    const int vsegs = std::max(segments, 2);
    const int hsegs = vsegs * 2;

    nverts_ = 1 + (vsegs - 1) * (hsegs + 1) + 1;       // top + middle + bottom
    ntris_ = hsegs + (vsegs - 2) * hsegs * 2 + hsegs;  // top + middle + bottom
    vertexarray_.resize(nverts_ * 8);
    indexarray_.resize(ntris_ * 3);

    const int stride = 8;  // the stride is based on the position, the normal, and texture coord

    // The vertex array: 3D xyz, 3D normal, 2D st (8 floats per vertex)
    // First vertex: top pole (+z is "up" in object local coords)
    vertexarray_[0] = 0.0f;
    vertexarray_[1] = 0.0f;
    vertexarray_[2] = radius;
    vertexarray_[3] = 0.0f;
    vertexarray_[4] = 0.0f;
    vertexarray_[5] = 1.0f;
    vertexarray_[6] = 0.5f;
    vertexarray_[7] = 1.0f;
    // Last vertex: bottom pole
    int base = (nverts_ - 1) * stride;
    vertexarray_[base] = 0.0f;
    vertexarray_[base + 1] = 0.0f;
    vertexarray_[base + 2] = -radius;
    vertexarray_[base + 3] = 0.0f;
    vertexarray_[base + 4] = 0.0f;
    vertexarray_[base + 5] = -1.0f;
    vertexarray_[base + 6] = 0.5f;
    vertexarray_[base + 7] = 0.0f;
    // All other vertices:
    // vsegs-1 latitude rings of hsegs+1 vertices each
    // (duplicates at texture seam s=0 / s=1)

    for (int j = 0; j < vsegs - 1; j++) {  // vsegs-1 latitude rings of vertices
        const double theta = static_cast<double>(j + 1) / vsegs * M_PI;
        const float z = static_cast<float>(std::cos(theta));
        const float R = static_cast<float>(std::sin(theta));

        for (int i = 0; i <= hsegs;
             i++) {  // hsegs+1 vertices in each ring (duplicate for texcoords)
            const double phi = static_cast<double>(i) / hsegs * 2.0 * M_PI;
            const float x = R * static_cast<float>(std::cos(phi));
            const float y = R * static_cast<float>(std::sin(phi));
            base = (1 + j * (hsegs + 1) + i) * stride;
            vertexarray_[base] = radius * x;
            vertexarray_[base + 1] = radius * y;
            vertexarray_[base + 2] = radius * z;
            vertexarray_[base + 3] = x;
            vertexarray_[base + 4] = y;
            vertexarray_[base + 5] = z;
            vertexarray_[base + 6] = (float)i / hsegs;
            vertexarray_[base + 7] = 1.0f - (float)(j + 1) / vsegs;
        }
    }

    // The index array: triplets of integers, one for each triangle
    // Top cap
    for (int i = 0; i < hsegs; i++) {
        indexarray_[3 * i] = 0;
        indexarray_[3 * i + 1] = 1 + i;
        indexarray_[3 * i + 2] = 2 + i;
    }
    // Middle part (possibly empty if vsegs=2)
    for (int j = 0; j < vsegs - 2; j++) {
        for (int i = 0; i < hsegs; i++) {
            base = 3 * (hsegs + 2 * (j * hsegs + i));
            const int i0 = 1 + j * (hsegs + 1) + i;
            indexarray_[base] = i0;
            indexarray_[base + 1] = i0 + hsegs + 1;
            indexarray_[base + 2] = i0 + 1;
            indexarray_[base + 3] = i0 + 1;
            indexarray_[base + 4] = i0 + hsegs + 1;
            indexarray_[base + 5] = i0 + hsegs + 2;
        }
    }
    // Bottom cap
    base = 3 * (hsegs + 2 * (vsegs - 2) * hsegs);
    for (int i = 0; i < hsegs; i++) {
        indexarray_[base + 3 * i] = nverts_ - 1;
        indexarray_[base + 3 * i + 1] = nverts_ - 2 - i;
        indexarray_[base + 3 * i + 2] = nverts_ - 3 - i;
    }

    // Generate one vertex array object (VAO) and bind it
    glGenVertexArrays(1, &(vao_));
    glBindVertexArray(vao_);

    // Generate two buffer IDs
    glGenBuffers(1, &vertexbuffer_);
    glGenBuffers(1, &indexbuffer_);

    // Activate the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_);
    // Present our vertex coordinates to OpenGL
    glBufferData(GL_ARRAY_BUFFER, vertexarray_.size() * sizeof(GLfloat), vertexarray_.data(),
                 GL_STATIC_DRAW);
    // Specify how many attribute arrays we have in our VAO
    glEnableVertexAttribArray(0);  // Vertex coordinates
    glEnableVertexAttribArray(1);  // Normals
    glEnableVertexAttribArray(2);  // Texture coordinates
    // Specify how OpenGL should interpret the vertex buffer data:
    // Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
    // Number of dimensions (3 means vec3 in the shader, 2 means vec2)
    // Type GL_FLOAT
    // Not normalized (GL_FALSE)
    // Stride 8 (interleaved array with 8 floats per vertex)
    // Array buffer offset 0, 3, 6 (offset into first vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)0);  // xyz coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));  // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(6 * sizeof(GLfloat)));  // texcoords

    // Activate the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer_);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexarray_.size() * sizeof(GLuint), indexarray_.data(),
                 GL_STATIC_DRAW);

    // Deactivate (unbind) the VAO and the buffers again.
    // Note that the order of these operations matter:
    // do NOT unbind the buffers while the VAO is still bound.
    // The index buffer is an essential part of the VAO state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*
 * readObj(const char* filename)
 *
 * Load TriangleSoup geometry data from an OBJ file.
 * The vertex array is on interleaved format. For each vertex, there
 * are 8 floats: three for the vertex coordinates (x, y, z), three
 * for the normal vector (n_x, n_y, n_z) and finally two for texture
 * coordinates (s, t). The arrays are allocated by "new" inside the
 * function and should be disposed of using "delete" when they are no longer
 * needed. This is done by the method clean() called by the destructor.
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2014.
 * This code is in the public domain.
 */
void TriangleSoup::readOBJ(const std::string& filename) {
    FILE* objfile = fopen(filename.c_str(), "r");

    if (!objfile) {
        std::cerr << "File not found: " << filename << "\n";
        return;
    }

    // Scan through the file to count the number of data elements
    char line[256];
    char tag[3];

    int numverts = 0;
    int numnormals = 0;
    int numtexcoords = 0;
    int numfaces = 0;
    while (fgets(line, 256, objfile)) {
        sscanf(line, "%2s ", tag);
        if (!strcmp(tag, "v")) {
            numverts++;
        } else if (!strcmp(tag, "vn")) {
            numnormals++;
        } else if (!strcmp(tag, "vt")) {
            numtexcoords++;
        } else if (!strcmp(tag, "f")) {
            numfaces++;
        }
        // else {
        //     std::cout << "Ignoring line starting with \"" << tag << "\"\n";
        // }
    }

    std::cout << "loadObj(\"" << filename << "\"): found " << numverts << " vertices, "
              << numnormals << " normals, " << numtexcoords << " texcoords, " << numfaces
              << " faces.\n";

    std::vector<float> verts(3 * numverts);
    std::vector<float> normals(3 * numnormals);
    std::vector<float> texcoords(2 * numtexcoords);

    vertexarray_.resize(8 * 3 * numfaces);
    indexarray_.resize(3 * numfaces);
    nverts_ = 3 * numfaces;
    ntris_ = numfaces;

    rewind(objfile);  // Start from the top again to read data

    int i_v = 0;
    int i_n = 0;
    int i_t = 0;
    int i_f = 0;

    int readerror = 0;
    while (fgets(line, 256, objfile)) {
        tag[0] = '\0';
        sscanf(line, "%2s ", tag);
        if (!strcmp(tag, "v")) {
            // A vertex with three coordinates
            // std::cout << "Reading vertex " << i_v + 1 << "\n";

            int numargs = sscanf(line, "v %f %f %f", &verts[3 * i_v], &verts[3 * i_v + 1],
                                 &verts[3 * i_v + 2]);
            if (numargs != 3) {
                std::cerr << "Malformed vertex data found at vertex " << i_v + 1 << "\nAborting\n";
                readerror = 1;
                break;
            }
            i_v++;
        } else if (!strcmp(tag, "vn")) {
            // A vertex normal with three components
            // std::cout << "Reading normal " << i_n + 1 << "\n";

            int numargs = sscanf(line, "vn %f %f %f", &normals[3 * i_n], &normals[3 * i_n + 1],
                                 &normals[3 * i_n + 2]);
            if (numargs != 3) {
                std::cerr << "Malformed normal data found at normal" << i_n + 1 << "\nAborting\n";
                readerror = 1;
                break;
            }
            i_n++;
        } else if (!strcmp(tag, "vt")) {
            // A vertex texture coordinate, two components
            // std::cout << "Reading texcoord " << i_t + 1 << "\n";

            int numargs = sscanf(line, "vt %f %f", &texcoords[2 * i_t], &texcoords[2 * i_t + 1]);
            if (numargs != 2) {
                std::cerr << "Malformed texcoord data found at texcoord " << i_t + 1
                          << "\nAborting\n";
                readerror = 1;
                break;
            }
            i_t++;
        } else if (!strcmp(tag, "f")) {
            // A face with three or more vertex indices
            // std::cout << "Reading face " << i_f + 1 << "\n";

            int v1, v2, v3, n1, n2, n3, t1, t2, t3;
            int numargs = sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &t1, &n1, &v2, &t2, &n2,
                                 &v3, &t3, &n3);
            if (numargs != 9) {  // Accept only triangles. Quads cause an error.
                std::cerr << "Malformed face data found at vertex " << i_f + 1 << "\nAborting\n";
                readerror = 1;
                break;
            }
            //			printf("Read vertex data %d/%d/%d %d/%d/%d %d/%d/%d\n",
            //			v1, t1, n1, v2, t2, n2, v3, t3, n3);
            // Indices in OBJ files start at 1, but C++ arrays start at index 0.
            --v1;
            --v2;
            --v3;
            --n1;
            --n2;
            --n3;
            --t1;
            --t2;
            --t3;

            const int currentv = 8 * 3 * i_f;
            vertexarray_[currentv] = verts[3 * v1];
            vertexarray_[currentv + 1] = verts[3 * v1 + 1];
            vertexarray_[currentv + 2] = verts[3 * v1 + 2];
            vertexarray_[currentv + 3] = normals[3 * n1];
            vertexarray_[currentv + 4] = normals[3 * n1 + 1];
            vertexarray_[currentv + 5] = normals[3 * n1 + 2];
            vertexarray_[currentv + 6] = texcoords[2 * t1];
            vertexarray_[currentv + 7] = texcoords[2 * t1 + 1];
            vertexarray_[currentv + 8] = verts[3 * v2];
            vertexarray_[currentv + 9] = verts[3 * v2 + 1];
            vertexarray_[currentv + 10] = verts[3 * v2 + 2];
            vertexarray_[currentv + 11] = normals[3 * n2];
            vertexarray_[currentv + 12] = normals[3 * n2 + 1];
            vertexarray_[currentv + 13] = normals[3 * n2 + 2];
            vertexarray_[currentv + 14] = texcoords[2 * t2];
            vertexarray_[currentv + 15] = texcoords[2 * t2 + 1];
            vertexarray_[currentv + 16] = verts[3 * v3];
            vertexarray_[currentv + 17] = verts[3 * v3 + 1];
            vertexarray_[currentv + 18] = verts[3 * v3 + 2];
            vertexarray_[currentv + 19] = normals[3 * n3];
            vertexarray_[currentv + 20] = normals[3 * n3 + 1];
            vertexarray_[currentv + 21] = normals[3 * n3 + 2];
            vertexarray_[currentv + 22] = texcoords[2 * t3];
            vertexarray_[currentv + 23] = texcoords[2 * t3 + 1];
            indexarray_[3 * i_f] = 3 * i_f;
            indexarray_[3 * i_f + 1] = 3 * i_f + 1;
            indexarray_[3 * i_f + 2] = 3 * i_f + 2;
            i_f++;
        }
    }

    // Clean up the temporary arrays we created
    fclose(objfile);

    if (readerror) {  // Delete corrupt data and bail out if a read error occured
        std::cerr << "Mesh read error: No mesh data generated\n";
        clean();
        return;
    }

    // Generate one vertex array object (VAO) and bind it
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    // Generate two buffer IDs
    glGenBuffers(1, &vertexbuffer_);
    glGenBuffers(1, &indexbuffer_);

    // Activate the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_);
    // Present our vertex coordinates to OpenGL
    glBufferData(GL_ARRAY_BUFFER, vertexarray_.size() * sizeof(GLfloat), vertexarray_.data(),
                 GL_STATIC_DRAW);

    // Specify how many attribute arrays we have in our VAO
    glEnableVertexAttribArray(0);  // Vertex coordinates
    glEnableVertexAttribArray(1);  // Normals
    glEnableVertexAttribArray(2);  // Texture coordinates
    // Specify how OpenGL should interpret the vertex buffer data:
    // Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
    // Number of dimensions (3 means vec3 in the shader, 2 means vec2)
    // Type GL_FLOAT
    // Not normalized (GL_FALSE)
    // Stride 8 (interleaved array with 8 floats per vertex)
    // Array buffer offset 0, 3, 6 (offset into first vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)0);  // xyz coordinates
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(3 * sizeof(GLfloat)));  // normals
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void*)(6 * sizeof(GLfloat)));  // texcoords

    // Activate the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer_);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexarray_.size() * sizeof(GLuint), indexarray_.data(),
                 GL_STATIC_DRAW);

    // Deactivate (unbind) the VAO and the buffers again.
    // Do NOT unbind the buffers while the VAO is still bound.
    // The index buffer is an essential part of the VAO state.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return;
}

/* Print data from a TriangleSoup object, for debugging purposes */
void TriangleSoup::print() {
    printf("TriangleSoup vertex data:\n\n");
    for (int i = 0; i < nverts_; i++) {
        printf("%d: %8.2f %8.2f %8.2f\n", i, vertexarray_[8 * i], vertexarray_[8 * i + 1],
               vertexarray_[8 * i + 2]);
    }
    printf("\nTriangleSoup face index data:\n\n");
    for (int i = 0; i < ntris_; i++) {
        printf("%d: %d %d %d\n", i, indexarray_[3 * i], indexarray_[3 * i + 1],
               indexarray_[3 * i + 2]);
    }
}

/* Print information about a TriangleSoup object (stats and extents) */
void TriangleSoup::printInfo() {
    printf("TriangleSoup information:\n");
    printf("vertices : %d\n", nverts_);
    printf("triangles: %d\n", ntris_);
    float xmin = vertexarray_[0];
    float xmax = xmin;
    float ymin = vertexarray_[1];
    float ymax = ymin;
    float zmin = vertexarray_[2];
    float zmax = zmin;

    for (int i = 1; i < nverts_; i++) {
        const float x = vertexarray_[8 * i];
        const float y = vertexarray_[8 * i + 1];
        const float z = vertexarray_[8 * i + 2];
        //         printf("x y z : %8.2f %8.2f %8.2f\n", x, y, z);

        xmin = std::min(xmin, x);
        xmax = std::max(xmax, x);
        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
        zmin = std::min(zmin, z);
        zmax = std::max(zmax, z);
    }
    printf("xmin: %8.2f\n", xmin);
    printf("xmax: %8.2f\n", xmax);
    printf("ymin: %8.2f\n", ymin);
    printf("ymax: %8.2f\n", ymax);
    printf("zmin: %8.2f\n", zmin);
    printf("zmax: %8.2f\n", zmax);
}

/* Render the geometry in a TriangleSoup object */
void TriangleSoup::render() {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 3 * ntris_, GL_UNSIGNED_INT, (void*)0);
    // (mode, vertex count, type, element array buffer offset)
    glBindVertexArray(0);
}
