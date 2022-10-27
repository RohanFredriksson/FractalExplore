#include "external.h"
#include "camera.h"
#include "window.h"

double projectionWidth;
double projectionHeight;
double xPos;
double yPos;
double zoom;

void Camera_Init() {
    xPos = 0.0;
    yPos = 0.0;
    projectionWidth = 4.0;
    projectionHeight = 4.0;
    zoom = 1.0;
    Camera_AdjustProjection();
}

void Camera_AdjustProjection() {
    projectionWidth = projectionHeight * Window_GetAspectRatio();
}

double Camera_GetProjectionWidth() {
    return projectionWidth;
}

double Camera_GetProjectionHeight() {
    return projectionHeight;
}

double Camera_GetX() {
    return xPos;
}

double Camera_GetY() {
    return yPos;
}

double Camera_GetZoom() {
    return zoom;
}

void Camera_SetX(double x) {
    xPos = x;
}

void Camera_SetY(double y) {
    yPos = y;
}

void Camera_SetZoom(double z) {
    zoom = z;
}