#pragma once

#include <glm/glm.hpp>

class Camera {

    private:

        glm::vec2 projectionSize;
        glm::mat4 projection, view, inverseProjection, inverseView;
        float zoom;

    public:

        glm::vec2 position;
        Camera(glm::vec2 position);
        
        void adjustProjection();
        glm::mat4 getView();
        glm::mat4 getProjection();
        glm::mat4 getInverseView();
        glm::mat4 getInverseProjection();
        glm::vec2 getProjectionSize();
        glm::vec2 getPosition();

        float getZoom();
        void setZoom(float z);
        void addZoom(float z);

};