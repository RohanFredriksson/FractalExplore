#pragma once

#include "core/window.hpp"
#include "core/arbitrary.hpp"

namespace Camera {
    
    void init();
    void adjust();
    
    Arbitrary getX();
    Arbitrary getY();
    Arbitrary getWidth();
    Arbitrary getHeight();
    Arbitrary getDepth();

    void setX(Arbitrary v);
    void setY(Arbitrary v);
    void setWidth(Arbitrary v);
    void setHeight(Arbitrary v);
    void setDepth(Arbitrary v);

}