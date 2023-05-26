#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "window.hpp"

Camera::Camera() {
    this->x = Arbitrary(0.0f);
    this->y = Arbitrary(0.0f);
    this->width = Arbitrary(2.0f);
    this->height = Arbitrary(2.0f);
    this->depth = Arbitrary(1.0f);
    this->adjust();
}

void Camera::adjust() {
    this->width = this->height * Arbitrary(Window::getAspectRatio());
}