#include <glm/ext.hpp>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "window.hpp"

Camera::Camera(glm::vec2 pos) {

    this->position = pos;
    this->projectionSize = glm::vec2(4.0f, 4.0f);
    this->projection = glm::mat4(1.0f);
    this->view = glm::mat4(1.0f);
    this->inverseProjection = glm::mat4(1.0f);
    this->inverseView = glm::mat4(1.0f);
    this->zoom = 0.5f;
    adjustProjection();

}
        
void Camera::adjustProjection() {
    
    //this->projectionSize.x = this->projectionSize.y * Window::getAspectRatio();   
    this->projection = glm::ortho(0.0f, this->projectionSize.x * this->zoom, 0.0f, this->projectionSize.y * this->zoom, 0.0f, 100.0f);
    this->inverseProjection = glm::inverse(this->projection);

}
        
glm::mat4 Camera::getView() {
    
    glm::vec3 cameraPos(this->position.x - this->projectionSize.x / 2 * this->zoom, this->position.y - this->projectionSize.y / 2 * this->zoom, 20.0f);
    glm::vec3 cameraFront(this->position.x - this->projectionSize.x / 2 * this->zoom, this->position.y - this->projectionSize.y / 2 * this->zoom, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
    this->view = glm::lookAt(cameraPos, cameraFront, cameraUp);

    return this->view;
}
        
glm::mat4 Camera::getProjection() {
    return this->projection;
}
        
glm::mat4 Camera::getInverseView() {
    return this->inverseView;
}
        
glm::mat4 Camera::getInverseProjection() {
    return this->inverseProjection;
}
        
glm::vec2 Camera::getProjectionSize() {
    return this->projectionSize;
}
        
glm::vec2 Camera::getPosition() {
    return this->position;
}
        
float Camera::getZoom() {
    return this->zoom;
}
        
void Camera::setZoom(float z) {
    this->zoom = z;
}
        
void Camera::addZoom(float z) {
    this->zoom = this->zoom + z;
}
        
