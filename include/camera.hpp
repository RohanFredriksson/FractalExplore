#pragma once

#include "arbitrary.hpp"

class Camera {

    public:

        Arbitrary x;
        Arbitrary y;
        Arbitrary width;
        Arbitrary height;
        Arbitrary zoom;

        Camera();
        void adjust();

};