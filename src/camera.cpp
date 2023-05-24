#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "camera.hpp"
//#include "window.hpp"

Camera::Camera() {
    this->x = Arbitrary(0.0f);
    this->y = Arbitrary(0.0f);
    this->width = Arbitrary(3.0f);
    this->height = Arbitrary(3.0f);
    this->zoom = Arbitrary(1.0f);
    this->adjust();
}

void Camera::adjust() {
    this->width = this->width; // * ASPECT RATIO
}