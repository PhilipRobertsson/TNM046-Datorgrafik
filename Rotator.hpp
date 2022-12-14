/* Rotator.hpp
 * Two classes to perform viewport rotations on mouse and keyboard input with
 * GLFW
 *
 * Usage: call init() before the rendering loop, call poll() once per frame,
 * read public members phi and theta to construct a rotation matrix.
 * The suggested composite rotation matrix is RotX(theta)*RotY(phi).
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2013-2015
 *          Martin Falk (martin.falk@liu.se) 2021
 */
#pragma once

struct GLFWwindow;

class KeyRotator {
public:
    KeyRotator(GLFWwindow* window);

    void poll();

    double phi() const;
    double theta() const;

private:
    GLFWwindow* window_;

    double phi_;
    double theta_;
    double lastTime_;
};

class MouseRotator {
public:
    MouseRotator(GLFWwindow* window);

    void poll();

    double phi() const;
    double theta() const;

private:
    GLFWwindow* window_;

    double phi_;
    double theta_;

    double lastX_;
    double lastY_;
    bool leftPressed_;
    bool rightPressed_;
};
