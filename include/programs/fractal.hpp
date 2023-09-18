#pragma once

#include <imgui.h>

#include "util.hpp"
#include "core/window.hpp"
#include "core/camera.hpp"
#include "core/arbitrary.hpp"
#include "programs/program.hpp"

class FractalProgram : public ShaderProgram {

    private:

        int precision = 1;
        int iterations = 64;

    public:

        void compile() override {

            if (this->program != nullptr) {delete this->program;}

            std::string v = this->vertex();
            std::string f = this->fragment();

            std::string search = "${PRECISION}";
            size_t position = f.find(search);
            if (position != std::string::npos) {f.replace(position, search.length(), std::to_string(this->precision+1));}

            this->program = new Shader(v, f);

        } 

        virtual void upload() override {
            
            Shader* s = this->getShader();
            Arbitrary width = Arbitrary(0.5f) * Camera::getDepth() * Camera::getWidth();
			Arbitrary height = Arbitrary(0.5f) * Camera::getDepth() * Camera::getHeight();
        
			s->uploadInt("uIterations", this->iterations);
			s->uploadUnsignedIntArray("uPositionX", Arbitrary::precision()+1, Camera::getX().data());
			s->uploadUnsignedIntArray("uPositionY", Arbitrary::precision()+1, Camera::getY().data());
			s->uploadUnsignedIntArray("uScaleX", Arbitrary::precision()+1, width.data());
			s->uploadUnsignedIntArray("uScaleY", Arbitrary::precision()+1, height.data());

        }

        virtual void imgui() override {

            int next = this->precision;
			ImGui::InputInt("Precision", &next);
			if (next != precision && next > 0 && next < Arbitrary::precision()) {
				this->precision = next;
				this->compile();
                Window::update();
			}

			next = this->iterations;
			ImGui::InputInt("Iterations", &next);
			if (next != iterations && next >= 0) {
				this->iterations = next;
				Window::update();
			}

        }

};