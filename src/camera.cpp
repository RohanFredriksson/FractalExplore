#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "window.hpp"

Camera::Camera() {
    this->x = Arbitrary(0.0);
    this->y = Arbitrary(0.0);
    this->width = Arbitrary(2.0);
    this->height = Arbitrary(2.0);
    this->depth = Arbitrary(1.0);
    this->adjust();
}

void Camera::adjust() {
    this->width = this->height * Arbitrary(Window::getAspectRatio());
}