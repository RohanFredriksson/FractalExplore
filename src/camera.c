#include "external.h"
#include "camera.h"
#include "window.h"

void Camera_Init(Camera* c) {
    
    c->xPos = 0.0;
    c->yPos = 0.0;
    c->projectionWidth = 0.0;
    c->projectionHeight = 2.0;
    c->zoom = 1.0;
    Camera_AdjustProjection(c);

}

void Camera_AdjustProjection(Camera* c) {
    c->projectionWidth = c->projectionHeight * Window_GetAspectRatio();
}