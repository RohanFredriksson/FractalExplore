#pragma once

#include "camera.hpp"

namespace Window {

    int getWidth();
    int getHeight();
    float getAspectRatio();
    Camera* getCamera();

    void setWidth(int width);
    void setHeight(int height);
    void resetFramebuffers();

};