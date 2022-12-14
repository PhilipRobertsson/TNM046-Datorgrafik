/*
 * A class to manage an OpenGL texture, and load texture data from a TGA file.
 *
 * Modified, stripped-down and cleaned-up version of the TGA loader from NeHe tutorial 33.
 *
 * Usage: Call createTexture() with a TGA file as argument to load a texture,
 *        or use the constructor with a file name argument. Uncompressed RGB or RGBA only.
 *        Call glBindTexture() with the public member textureID as argument.
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2014
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class Texture {
public:

    /* Constructor to load and intialize the texture all at once */
    Texture(const std::string& filename = "");

    /* Destructor */
    ~Texture();

    // The external entry point for loading a texture from a TGA file
    void createTexture(const std::string& filename);  // Load GL texture from file

    // returns the OpenGL texture ID
    GLuint id() const;

    GLuint width() const;
    GLuint height() const;

    // returns the type of the texture (GL_RGB or GL_RGBA)
    GLuint type() const;

private:
    struct ImageData {
        GLuint width = 0;                // Image width
        GLuint height = 0;               // Image height
        GLuint type = 0;                 // Image type (3 bytes per pixel: GL_RGB, 4 bytes: GL_RGBA)
        std::vector<GLubyte> data;  // Image data (3 or 4 bytes per pixel)
    };

    // Load data from an uncompressed TGA file
    ImageData loadUncompressedTGA(const std::string& filename) const;

    GLuint textureID_;  // Texture ID for OpenGL
    ImageData image_;
};
