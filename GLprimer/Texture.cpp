/*
 * OpenGL texture, and load texture data from a TGA file.
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2014
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#include <cstdio>   // For file I/O
#include <cstring>  // For memcmp()
#include <iostream>
#include <fstream>
#include <algorithm>
#include <array>

#include <GL/glew.h>

#include "Texture.hpp"

/* Constructor to load and intialize the texture all at once */
Texture::Texture(const std::string& filename) : textureID_(0) { createTexture(filename); }

/* Destructor */
Texture::~Texture() {
    if (textureID_ != 0) {
        glDeleteTextures(1, &textureID_);
    }
}

GLuint Texture::id() const { return textureID_; }

GLuint Texture::width() const { return image_.width; }

GLuint Texture::height() const { return image_.height; }

GLuint Texture::type() const { return image_.type; }

/*
 * Open and test the file to make sure it is a valid TGA file
 *
 * roughly based on NeHe's TGA loading code
 */
Texture::ImageData Texture::loadUncompressedTGA(const std::string& filename) const {
    std::ifstream in(filename, std::ios_base::in | std::ios_base::binary);

    if (!in.is_open()) {
        std::cerr << "Could not open texture file ('" << filename << "')\n";
        return {};  // return an empty image
    }

    // Attempt to read 12 byte file header
    std::array<char, 12> tgaheader;

    in.read(tgaheader.data(), sizeof(tgaheader));
    if (in.fail()) {
        std::cerr << "Could not read file header ('" << filename << "')\n";
        return {};  // return an empty image
    }

    // headers for compressed and uncompressed TGAs
    const std::array<char, 12> uncompressedTGA = {{0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    const std::array<char, 12> compressedTGA = {{0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

    if (tgaheader == compressedTGA) {
        std::cerr << "RLE compressed TGA files are not supported ('" << filename << "')\n";
        return {};
    } else if (tgaheader != uncompressedTGA) {
        std::cerr << "Unsupported image file format ('" << filename << "')\n";
        return {};
    }

    std::array<GLubyte, 6> header;  // First 6 useful bytes from the header
    // std::ifstream::read() only reads 'char', thus we need a type cast for tga.header
    in.read(reinterpret_cast<char*>(header.data()), sizeof(header));
    if (in.fail()) {
        std::cerr << "Could not read TGA header ('" << filename << "')\n";
        return {};
    }

    ImageData image;

    // Determine the TGA width (highbyte*256 + lowbyte)
    image.width = header[1] * 256 + header[0];
    // Determine the TGA height	(highbyte*256 + lowbyte)
    image.height = header[3] * 256 + header[2];

    // Make sure all information is valid
    if ((image.width <= 0) || (image.height <= 0)) {
        std::cerr << "Invalid image dimensions ('" << filename << "')\n";
        return {};
    }

    // Determine the bits per pixel
    const GLuint bpp = header[4];
    // Compute the number of BYTES per pixel
    const GLuint bytesPerPixel = (bpp / 8);
    // Compute the total amount of memory needed
    const GLuint imageSize = (bytesPerPixel * image.width * image.height);

    switch (bpp) {
        case 24:
            image.type = GL_RGB;
            std::cout << "Texture type is GL_RGB ('" << filename << "')\n";
            break;
        case 32:
            image.type = GL_RGBA;
            std::cout << "Texture type is GL_RGB ('" << filename << "')\n";
            break;
        default:
            std::cerr << "Unsupported number of bits per pixel (" << bpp << ") ('" << filename
                      << "')\n";
            return {};
    }

    image.data.resize(imageSize);  // Allocate memory for image data

    // Attempt to read image data
    // std::ifstream::read() only reads 'char', thus we need a type cast for tga.header
    in.read(reinterpret_cast<char*>(image.data.data()), imageSize);
    if (in.gcount() != imageSize) {
        std::cerr << "Could not read image data ('" << filename << "')\n";
        return {};
    }

    // Swap the BGR(A) byte order in the TGA file to the standard RGB(A) byte order for OpenGL.
    for (size_t cswap = 0; cswap < imageSize; cswap += bytesPerPixel) {
        std::swap(image.data[cswap], image.data[cswap + 2]);
    }

    return image;
}

/*
 * Load and activate a 2D texture from a TGA file
 */
void Texture::createTexture(const std::string& filename) {
    image_ = loadUncompressedTGA(filename);

    if (image_.data.empty()) {
        return;
    }

    if (textureID_ == 0) {
        glGenTextures(1, &textureID_);  // Create the texture ID if it does not exist
    }

    glBindTexture(GL_TEXTURE_2D, textureID_);
    // Set parameters to determine how the texture is resized
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Set parameters to determine how the texture wraps at edges
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Read the texture data from file and upload it to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_.width, image_.height, 0, image_.type,
                 GL_UNSIGNED_BYTE, image_.data.data());

    glEnable(GL_TEXTURE_2D);  // Required for glGenerateMipmap() to work
    glGenerateMipmap(GL_TEXTURE_2D);

    // Image data was copied to the GPU, release contents of the std::vector
    // When using clear() the std::vector would still hold on to the memory.
    image_.data = std::vector<GLubyte>();
}
