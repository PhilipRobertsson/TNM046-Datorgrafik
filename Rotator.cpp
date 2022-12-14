/* Rotator.hpp
 * Two classes to perform viewport rotations on mouse and keyboard input with
 * GLFW
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2013-2015
 *          Martin Falk (martin.falk@liu.se) 2021
 */
#if defined(WIN32) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

#include "Rotator.hpp"

#include <GLFW/glfw3.h>
#include <cmath>

KeyRotator::KeyRotator(GLFWwindow* window)
    : window_(window), phi_(0.0), theta_(0.0), lastTime_(glfwGetTime()) {}

void KeyRotator::poll() {
    const double currentTime = glfwGetTime();
    const double elapsedTime = currentTime - lastTime_;
    lastTime_ = currentTime;

    if (glfwGetKey(window_, GLFW_KEY_RIGHT)) {
        phi_ += elapsedTime * M_PI / 2.0;  // Rotate 90 degrees per second (pi/2)
        phi_ = fmod(phi_, M_PI * 2.0);     // Wrap around at 360 degrees (2*pi)
    }

    if (glfwGetKey(window_, GLFW_KEY_LEFT)) {
        phi_ -= elapsedTime * M_PI / 2.0;  // Rotate 90 degrees per second (pi/2)
        phi_ = fmod(phi_, M_PI * 2.0);
        if (phi_ < 0.0) {
            phi_ += M_PI * 2.0;  // If phi_<0, then fmod(phi_,2*pi)<0
        }
    }

    if (glfwGetKey(window_, GLFW_KEY_UP)) {
        theta_ += elapsedTime * M_PI / 2.0;  // Rotate 90 degrees per second
        if (theta_ >= M_PI / 2.0) {
            theta_ = M_PI / 2.0;  // Clamp at 90
        }
    }

    if (glfwGetKey(window_, GLFW_KEY_DOWN)) {
        theta_ -= elapsedTime * M_PI / 2.0;  // Rotate 90 degrees per second
        if (theta_ < -M_PI / 2.0) {
            theta_ = -M_PI / 2.0;  // Clamp at -90
        }
    }
}

double KeyRotator::phi() const { return phi_; }

double KeyRotator::theta() const { return theta_; }

MouseRotator::MouseRotator(GLFWwindow* window)
    : window_(window), phi_(0.0), theta_(0.0), leftPressed_(false), rightPressed_(false) {
    glfwGetCursorPos(window, &lastX_, &lastY_);
}

void MouseRotator::poll() {
    // Find out where the mouse pointer is, and which buttons are pressed
    double currentX;
    double currentY;
    glfwGetCursorPos(window_, &currentX, &currentY);

    bool currentLeft = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool currentRight = glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    if (currentLeft && leftPressed_) {  // If a left button drag is in progress
        int windowWidth;
        int windowHeight;
        glfwGetWindowSize(window_, &windowWidth, &windowHeight);

        const double moveX = currentX - lastX_;
        const double moveY = currentY - lastY_;

        phi_ += M_PI * moveX / windowWidth;  // Longest drag rotates 180 degrees
        if (phi_ >= M_PI * 2.0) {
            phi_ = fmod(phi_, M_PI * 2.0);
        }
        if (phi_ < 0.0) {
            phi_ += M_PI * 2.0;
        }                                       // If phi<0, then fmod(phi,2*pi)<0
        theta_ += M_PI * moveY / windowHeight;  // Longest drag rotates 180 deg
        if (theta_ >= M_PI / 2.0) {
            theta_ = M_PI / 2.0;  // Clamp at 90
        }
        if (theta_ < -M_PI / 2.0) {
            theta_ = -M_PI / 2.0;  // Clamp at 90
        }
    }

    leftPressed_ = currentLeft;
    rightPressed_ = currentRight;
    lastX_ = currentX;
    lastY_ = currentY;
}

double MouseRotator::phi() const { return phi_; }

double MouseRotator::theta() const { return theta_; }
