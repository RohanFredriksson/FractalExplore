#pragma once

#include "arbitrary.hpp"

class Camera {

    public:

        Arbitrary x;
        Arbitrary y;
        Arbitrary width;
        Arbitrary height;
        Arbitrary depth;

        Camera();
        void adjust();

};