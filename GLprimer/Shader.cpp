/*
 * GLSL Shader loading and compilation
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2014
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.hpp"

#include <iostream>
#include <fstream>

Shader::Shader() : programID_(0) {}

Shader::Shader(const std::string& vertexshaderfile, const std::string& fragmentshaderfile) {
    createShader(vertexshaderfile, fragmentshaderfile);
}

Shader::~Shader() {
    if (programID_ != 0) {
        glDeleteProgram(programID_);  // free program resources
    }
}

GLuint Shader::id() const { return programID_; }

std::string readFile(const std::string& filename) {
    std::ifstream in(filename.c_str());
    if (!in.is_open()) {
        std::cerr << "Error: Could not open shader file '" << filename << "'\n";
        return {};
    }

    // determine length of file
    in.seekg(0, std::ios_base::end);
    size_t fileLength = in.tellg();
    in.seekg(0);  // reset file stream

    std::string buffer;
    buffer.resize(fileLength + 1);
    in.read(buffer.data(), fileLength);
    if (in.bad()) {
        std::cerr << "Error: Could not read shader file '" << filename << "'\n";
        return {};
    }

    buffer[fileLength] = '\0';  // make sure the string is null terminated
    in.close();

    return buffer;
}

GLuint loadShader(GLenum shaderType, const std::string& filename) {
    GLuint shader = glCreateShader(shaderType);
    std::string shaderSource = readFile(filename);
    if (!shaderSource.empty()) {
        const char* source = shaderSource.c_str();
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
    }

    GLint shaderCompiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);

    if (shaderCompiled == GL_FALSE) {
        // something went wrong, print the shader log
        static char buf[4096] = {0};  // buffer for error messages from the GLSL compiler and linker
        glGetShaderInfoLog(shader, sizeof(buf), nullptr, buf);
        std::cerr << "Shader compile error ('" << filename << "'):\n" << buf << "\n";
    }

    return shader;
}

void Shader::createShader(const std::string& vertexshaderfile,
                          const std::string& fragmentshaderfile) {
    // If a program is already stored in this object, delete it
    if (programID_ != 0) {
        glDeleteProgram(programID_);
    }

    // Create the vertex shader.
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexshaderfile);
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentshaderfile);

    // Create a program object and attach the two compiled shaders.
    GLuint programObject = glCreateProgram();
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program object and print out the info log.
    glLinkProgram(programObject);

    GLint shadersLinked = GL_FALSE;
    glGetProgramiv(programObject, GL_LINK_STATUS, &shadersLinked);

    if (shadersLinked == GL_FALSE) {
        char buf[4096] = {0};
        glGetProgramInfoLog(programObject, sizeof(buf), nullptr, buf);
        std::cerr << "Shader program linker error:\n" << buf << "\n";
    }
    glDeleteShader(vertexShader);    // After successful linking,
    glDeleteShader(fragmentShader);  // these are no longer needed

    programID_ = programObject;  // Save this value in the class variable
}
