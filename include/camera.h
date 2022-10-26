#include "external.h"

#ifndef CAMERA_H
#define CAMERA_H

void Camera_Init();
void Camera_AdjustProjection();

double Camera_GetProjectionWidth();
double Camera_GetProjectionHeight();
double Camera_GetX();
double Camera_GetY();
double Camera_GetZoom();

#endif