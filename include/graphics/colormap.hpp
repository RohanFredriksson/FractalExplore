#pragma once

#include <imgui.h>

#include "util.hpp"
#include "core/window.hpp"
#include "core/camera.hpp"
#include "core/arbitrary.hpp"
#include "graphics/shader.hpp"

class ColormapProgram : public ShaderProgram {

    public:

        void upload() override {
            Shader* s = this->getShader();
            s->uploadTexture("uTexture", 0);
        }

        void imgui() override {

        }

};