#include "core/camera.hpp"

namespace {

    Arbitrary x;
    Arbitrary y;
    Arbitrary width;
    Arbitrary height;
    Arbitrary depth;

}

namespace Camera {

    void init() {
        x = Arbitrary(0.0);
        y  = Arbitrary(0.0);
        width = Arbitrary(2.0);
        height = Arbitrary(2.0);
        depth = Arbitrary(1.0);
        adjust();
    }

    void adjust() {
        width = height * Arbitrary(Window::ratio());
    }

    Arbitrary getX() {
        return x;
    }

    Arbitrary getY() {
        return y;
    }

    Arbitrary getWidth() {
        return width;
    }

    Arbitrary getHeight() {
        return height;
    }   
    
    Arbitrary getDepth() {
        return depth;
    }

    void setX(Arbitrary v) {
        x = v;
    }

    void setY(Arbitrary v) {
        y = v;
    }

    void setWidth(Arbitrary v) {
        width = v;
    }

    void setHeight(Arbitrary v) {
        height = v;
    }

    void setDepth(Arbitrary v) {
        depth = v;
    }

}


