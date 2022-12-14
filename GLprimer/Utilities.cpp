/*
 * Some utility functions for the course TNM046
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2014
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#include "Utilities.hpp"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>

namespace util {

    double displayFPS(GLFWwindow* window) {
        static int frames = 0;
        static double fps = 0.0;

        static double t0 = glfwGetTime();  // Gets number of seconds since glfwInit()

        double t = glfwGetTime();  // Get current time

        // update fps only once every second
        if (t - t0 >= 1.0) {
            fps = static_cast<double>(frames) / (t - t0);
            t0 = t;
            frames = 0;
        }

        // update the window title
        if (frames == 0) {
            char title[201];
            // convert fps to milliseconds
            double frametime = (fps > 0.0) ? 1000.0 / fps : 0.0;
            snprintf(title, 200, "TNM046: %.2f ms/frame (%.1f FPS)", frametime, fps);
            glfwSetWindowTitle(window, title);
        }

        ++frames;
        return fps;
    }
}  // namespace util
