#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "window.hpp"

Camera::Camera() {
    this->x = Arbitrary(-0.75f);
    this->y = Arbitrary(0.1f);
    this->width = Arbitrary(0.01f);
    this->height = Arbitrary(0.01f);
    this->zoom = Arbitrary(1.0f);
    this->adjust();
}

void Camera::adjust() {
    this->width = this->width * Arbitrary(Window::getAspectRatio());
}