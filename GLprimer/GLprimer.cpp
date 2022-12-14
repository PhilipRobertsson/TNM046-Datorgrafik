/*
 * A C++ framework for OpenGL programming in TNM046 for MT1
 *
 * This is a small, limited framework, designed to be easy to use for students in an introductory
 * computer graphics course in the first year of a M Sc curriculum. It uses custom code for some
 * things that are better solved by external libraries like GLM, but the emphasis is on simplicity
 * andreadability, not generality.
 *
 * For the window management, GLFW 3.x is used for convenience.
 * The framework should work in Windows, MacOS X and Linux.
 * GLEW is used for handling platform specific OpenGL extensions.
 * This code depends only on GLFW, GLEW, and OpenGL libraries.
 * OpenGL 3.3 or higher is required.
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2013-2015
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#if defined(WIN32) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

// File and console I/O for logging and error reporting
#include <iostream>
// Math header for trigonometric functions
#include <cmath>

// Include vectors
#include <vector>

// Include array
#include <array>

// glew provides easy access to advanced OpenGL functions and extensions
#include <GL/glew.h>

// GLFW 3.x, to handle the OpenGL window
#include <GLFW/glfw3.h>

#include "Utilities.hpp"

#include "TriangleSoup.hpp"

#include "Texture.hpp"

#include "Rotator.hpp"

// Include shaders
#include "Shader.hpp"
// Functions
GLuint createVertexBuffer(int location, int dimensions, const std::vector<float>& vertices) {
    GLuint bufferID;
    // Generate buffer, activate it and copy the data
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    // Tell OpenGL how the data is stored in our buffer
    // Attribute location (must match layout(location=#) statement in shader)
    // Number of dimensions (3 -> vec3 in the shader, 2-> vec2 in the shader),
    // type GL_FLOAT, not normalized, stride 0, start at element 0
    glVertexAttribPointer(location, dimensions, GL_FLOAT, GL_FALSE, 0, nullptr);
    // Enable the attribute in the currently bound VAO
    glEnableVertexAttribArray(location);

    return bufferID;
}
GLuint createIndexBuffer(const std::vector<unsigned int>& indices) { 
    GLuint bufferID;
    // Generate buffer, activate it and copy the data
    glGenBuffers(1, &bufferID);
    // Activate (bind) the index buffer and copy data to it.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                 GL_STATIC_DRAW);
    return bufferID;
}
// Print the elements of a matrix m
void mat4print(const std::array<float, 16>& m) {
    printf("Matrix:\n");
    printf("%6.2f %6.2f %6.2f %6.2f\n", m[0], m[4], m[8], m[12]);
    printf("%6.2f %6.2f %6.2f %6.2f\n", m[1], m[5], m[9], m[13]);
    printf("%6.2f %6.2f %6.2f %6.2f\n", m[2], m[6], m[10], m[14]);
    printf("%6.2f %6.2f %6.2f %6.2f\n", m[3], m[7], m[11], m[15]);
    printf("\n");
}
//Matrix multiplication
// Multiply 4x4 matrices m1 and m2 and return the result
std::array<float, 16> mat4mult(const std::array<float, 16>& m1, const std::array<float, 16>& m2) {
    std::array<float, 16> result;

    result[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    result[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    result[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    result[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

    result[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    result[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    result[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    result[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

    result[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    result[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    result[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    result[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

    result[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
    result[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
    result[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
    result[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

    return result;
}

std::array<float, 16> mat4identity() {
    return {1.0f, 0.0f, 0.0f, 0.0f, 
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f, 
                0.0f, 0.0f, 0.0f, 1.0f
    };
}
std::array<float, 16> mat4rotx(float angle) {
    std::array<float, 16> temp = {
        1.0f,           0.0f,         0.0f, 0.0f,
        0.0f, cos(angle), sin(angle), 0.0f, 
        0.0f,-sin(angle), cos(angle), 0.0f,
        0.0f,          0.0f,          0.0f, 1.0f
    };
    return temp;
}
std::array<float, 16> mat4roty(float angle) {
    std::array<float, 16> temp = {
        cos(angle),  0.0f, sin(angle), 0.0f,
        0.0f,            1.0f,         0.0f, 0.0f,
        -sin(angle),  0.0f, cos(angle), 0.0f,
        0.0f,           0.0f,           0.0f, 1.0f
    };
    return temp;
}
std::array<float, 16> mat4rotz(float angle) {
    std::array<float, 16> temp = {
        cos(angle), sin(angle), 0.0f,  0.0f,
        -sin(angle), cos(angle),0.0f, 0.0f,       
        0.0f,                   0.0f, 1.0f, 0.0f,
        0.0f,                   0.0f, 0.0f, 1.0f
    };
    return temp;
}
std::array<float, 16> mat4scale(float scale) {
    return {scale, 0.0f, 0.0f, 0.0f,
                0.0f, scale, 0.0f, 0.0f,
                0.0f, 0.0f, scale, 0.0f,
                0.0f, 0.0f, 0.0f,  1.0f
    };
}
std::array<float, 16> mat4translate(float x, float y, float z) {
    return {1.0f, 0.0f, 0.0f, 0.0f, 
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f, 
                x,          y,      z, 1.0f
    };
}
std::array<float, 16> mat4perspective(float vfov, float aspect, float znear, float zfar) {
    float f = 1/tan(vfov/2);
    return {(f/aspect), 0.0f, 0.0f, 0.0f, 
                0.0f,       f,          0.0f, 0.0f,
                0.0f, 0.0f, -((zfar+znear)/(zfar-znear)),    -1.0f, 
                0.0f, 0.0f, -((2*znear*zfar)/(zfar-znear)), 0.0f
    };
}

    /*
 * main(int argc, char* argv[]) - the standard C++ entry point for the program
 */
int main(int, char*[]) {
    Shader myTrexShader;
    Shader mySphereShader;
    // Vertex coordinates (x,y,z) for three vertices
    const std::vector<GLfloat> vertexArrayData = {
        /*
        -1.0f, -1.0f, -1.0f, 
        -1.0f, -1.0f, -1.0f, 
        -1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,   1.0f, 
        -1.0f, -1.0f,   1.0f,  
        -1.0f, -1.0f,   1.0f,

        -1.0f,  1.0f,   1.0f,
        -1.0f,  1.0f,   1.0f,  
        -1.0f,  1.0f,   1.0f,

        -1.0f,  1.0f,  -1.0f, 
        -1.0f,  1.0f,  -1.0f, 
        -1.0f,  1.0f,  -1.0f,

         1.0f, -1.0f,  -1.0f, 
         1.0f, -1.0f,  -1.0f, 
         1.0f, -1.0f,  -1.0f,

         1.0f, -1.0f,   1.0f,  
         1.0f, -1.0f,   1.0f,  
         1.0f, -1.0f,   1.0f,

         1.0f,  1.0f,   1.0f,  
         1.0f,  1.0f,   1.0f,  
         1.0f,  1.0f,   1.0f,

         1.0f,  1.0f,  -1.0f,
         1.0f,  1.0f,  -1.0f, 
         1.0f,  1.0f,  -1.0f
         */
    };
    const std::vector<GLfloat> colorArrayData = {
        /*
       //P0
        1.0f, 0.31f, 0.5f,                  //0
        0.29f, 0.25f, 0.16f,             //1
        0.0f, 0.0f, 0.0f,                   //2
        //P1
        1.0f, 0.31f, 0.5f,                  //3
        0.88f, 0.88f, 0.08f,             //4
        0.0f, 0.0f, 0.0f,                   //5
        //P2
        1.0f, 0.31f, 0.5f,                  //6
        0.88f, 0.88f, 0.08f,            // 7
        0.53f, 0.67f, 0.03f,            //8
        //P3
        1.0f, 0.31f, 0.5f,                  //9
        0.29f, 0.25f, 0.16f,            //10
        0.53f, 0.67f, 0.03f,            // 11
        //P4
        0.29f, 0.25f, 0.16f,           // 12
        0.48f, 0.35f,0.02f,            //13
        0.0f,0.0f,0.0f,                   //14
        //P5
        0.48f, 0.35f, 0.02f,           // 15
        0.0f,0.0f,0.0f,                   //16
        0.88f, 0.88f, 0.08f,          // 17
        //P6
        0.48f, 0.35f, 0.02f,          // 18
        0.53f, 0.67f, 0.03f,         // 19
        0.88f, 0.88f, 0.08f,         // 20
        //P7
        0.48f, 0.35f, 0.02f,        // 21
        0.53f, 0.67f, 0.03f,        // 22
        0.29f, 0.25f, 0.16f,         // 23
        */
    };

    const std::vector<GLuint> indexArrayData = {
        /*
        0, 3, 9, 
        3,6, 9, 
        4, 17, 7,
        17, 20, 7, 
        15, 21, 18, 
        15, 13, 21, 
        12, 10, 23, 
        12, 1, 10, 
        8,22,11,
        8,19,22,
        2,16,5,
        2,14,16
        */
    };

    
    // Initialise GLFW
    glfwInit();

    const GLFWvidmode* vidmode;  // GLFW struct to hold information about the display
    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Make sure we are getting a GL context of at least version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable the OpenGL core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Open a square window (aspect 1:1) to fill half the screen height
    GLFWwindow* window =
        glfwCreateWindow(vidmode->height / 2, vidmode->height / 2, "GLprimer", nullptr, nullptr);
    if (!window) {
        std::cout << "Unable to open window. Terminating.\n";
        glfwTerminate();  // No window was opened, so we can't continue in any useful way
        return -1;
    }

    // Make the newly created window the "current context" for OpenGL
    // (This step is strictly required or things will simply not work)
    glfwMakeContextCurrent(window);

    // Initialize glew
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << "\n";
        glfwTerminate();
        return -1;
    }

    // Show some useful information on the GL context
    std::cout << "GL vendor:       " << glGetString(GL_VENDOR)
              << "\nGL renderer:     " << glGetString(GL_RENDERER)
              << "\nGL version:      " << glGetString(GL_VERSION)
              << "\nDesktop size:    " << vidmode->width << " x " << vidmode->height << "\n";

    // Get window size. It may start out different from the requested size and
    // will change if the user resizes the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // Set viewport. This is the pixel rectangle we want to draw into
    glViewport(0, 0, width, height);  // The entire window

    glfwSwapInterval(0);  // Do not wait for screen refresh between frames
   
    // Generate 1 Vertex array object, put the resulting identifier in vertexArrayID
    GLuint vertexArrayID = 0;
    glGenVertexArrays(1, &vertexArrayID);
    // Activate the vertex array object
    glBindVertexArray(vertexArrayID);

    // Create the vertex buffer objects for attribute locations 0 and 1
    // (the list of vertex coordinates and the list of vertex colors).
    GLuint vertexBufferID = createVertexBuffer(0, 3, vertexArrayData);
    GLuint colorBufferID = createVertexBuffer(1, 3, colorArrayData);
    // Create the index buffer object (the list of triangles).
    GLuint indexBufferID = createIndexBuffer(indexArrayData);

    // Deactivate the vertex array object again to be nice
    glBindVertexArray(0);

    myTrexShader.createShader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    // Do this before the rendering loop
    GLint locationTime = glGetUniformLocation(myTrexShader.id(), "time");
    if (locationTime == -1) {  // If the variable is not found, -1 is returned
        std::cout << "Unable to locate variable 'time'in shader!\n";
    }

    //Generate Sphere
    TriangleSoup myTrex;
    TriangleSoup myShpere;
    TriangleSoup myBox;
    myTrex.readOBJ("meshes/trex.obj");
    myShpere.createSphere(0.4f, 50);
    myBox.createBox(1.0f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    mat4print(mat4perspective(M_PI / 4.0, 1.0f, 0.1f, 100.0f));

    // Locate the sampler2D uniform in the shader program
    GLint locationTex = glGetUniformLocation(myTrexShader.id(), "tex");
    // Generate one texture object with data from a TGA file
    Texture trexTexture;
    trexTexture.createTexture("textures/pyramid.tga");

    Texture earthTexture;
    earthTexture.createTexture("textures/earth.tga");

    Texture pyramidTexture;
    pyramidTexture.createTexture("textures/trex.tga");

    KeyRotator myKeyRotator(window);
    MouseRotator myMouseRotator(window);

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);

        util ::displayFPS(window);
        // Set the clear color to a dark gray (RGBA)
        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

        
        // Clear the color and depth buffers for drawing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* ---- Rendering code should go here ---- */
        
        /*
        glBindTexture(GL_TEXTURE_2D, pyramidTexture.id());
        myBox.render();
        */

        // restore previous state (no texture, no shader)
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);

        // Do this in the rendering loop to update the uniform variable "time"
        float time = static_cast<float>(glfwGetTime());  // Number of seconds since the program was started
        glUseProgram(myTrexShader.id());                            // Activate the shader to set its variables
        glUniform1f(locationTime, time);                        // Copy the value to the shader program
        myKeyRotator.poll();
        std::array<GLfloat, 16> matKey = mat4mult(mat4rotz(-myKeyRotator.phi()), mat4rotx(-myKeyRotator.theta()));
        myMouseRotator.poll();
        std::array<GLfloat, 16> matMouse = mat4mult(mat4rotz(myMouseRotator.phi()), mat4rotx(-myMouseRotator.theta()));

        glUseProgram(myTrexShader.id()); 
        std::array<GLfloat, 16> vTranslate = mat4translate(0.0f, 0.0f, -3.0f);
        std::array<GLfloat, 16> vRot = mat4rotx(10*(M_PI/100));

        std::array<GLfloat, 16> rSpin = mat4mult(mat4mult(mat4mult(vTranslate,mat4roty(1)),vRot),matKey);
        GLint locationR = glGetUniformLocation(myTrexShader.id(), "MV");
        glUseProgram(myTrexShader.id());  // Activate the shader to set its variables
        glUniformMatrix4fv(locationR, 1, GL_FALSE, rSpin.data());  // Copy the value
        
        glBindTexture(GL_TEXTURE_2D, trexTexture.id());
        myTrex.render();

        vRot = mat4rotx(5 * (M_PI / 100));
        std::array<GLfloat, 16> vOrbit = mat4roty((time / 4 * M_PI));
        std::array<GLfloat, 16> cT = mat4translate(0.0f,0.0f,0.8f);

         std::array<GLfloat, 16> matO = mat4mult(mat4mult(vOrbit, cT), vRot);

        rSpin = mat4mult(mat4mult(mat4mult(vTranslate, mat4roty(time/4*M_PI)), vRot),matO);
        locationR = glGetUniformLocation(myTrexShader.id(), "MV");
        glUseProgram(myTrexShader.id());  // Activate the shader to set its variables
        glUniformMatrix4fv(locationR, 1, GL_FALSE, rSpin.data());  // Copy the value


        glBindTexture(GL_TEXTURE_2D, earthTexture.id());
        myShpere.render();

        std::array<GLfloat, 16> Ilumination = mat4mult(matMouse,mat4identity());
        GLint locationT = glGetUniformLocation(myTrexShader.id(), "T");
        glUseProgram(myTrexShader.id());  // Activate the shader to set its variables
        glUniformMatrix4fv(locationT, 1, GL_FALSE, Ilumination.data());  // Copy the value

        std::array<GLfloat, 16> P = mat4perspective(M_PI/3.0, 1.0f, 0.1f,100.0f);
        GLint locationP = glGetUniformLocation(myTrexShader.id(), "P");
        glUseProgram(myTrexShader.id());  // Activate the shader to set its variables
        glUniformMatrix4fv(locationP, 1, GL_FALSE, P.data());  // Copy the value


        
        // Activate the vertex array object we want to draw (we may have several)
        glBindVertexArray(vertexArrayID);
        glUseProgram(myTrexShader.id());
        // Draw our triangle with 3 vertices.
        // When the last argument of glDrawElements is nullptr, it means
        // "use the previously bound index buffer". (This is not obvious.)
        // The index buffer is part of the VAO state and is bound with it.
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glCullFace(GL_BACK);

        // Draw again
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glCullFace(GL_BACK);




        // Swap buffers, display the image and prepare for next frame
        glfwSwapBuffers(window);

        // Poll events (read keyboard and mouse input)
        glfwPollEvents();

        // Exit if the ESC key is pressed (and also if the window is closed)
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
    // release the vertex and index buffers as well as the vertex array
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &colorBufferID);


    // Close the OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}

