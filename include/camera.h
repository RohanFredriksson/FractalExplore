#include "external.h"

#ifndef CAMERA_H
#define CAMERA_H

struct Camera {
    double projectionWidth;
    double projectionHeight;
    double xPos;
    double yPos;
    double zoom;
};
typedef struct Camera Camera;

void Camera_Init(Camera* c);
void Camera_AdjustProjection(Camera* c);

#endif