#pragma once

#include <imgui.h>
#include "programs/program.hpp"

class TransformationProgram : public ShaderProgram {

    public:

        virtual void upload() override {
            Shader* s = this->getShader();
            s->uploadTexture("uTexture", 0);
        }

        virtual void imgui() override {

        }

};