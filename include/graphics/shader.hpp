#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

#include "util.hpp"

class Shader {

    private:
        unsigned int program;

    public:

        Shader(std::string vertex, std::string fragment);
        ~Shader();

        void bind();
        void unbind();

        void uploadInt(std::string name, int value);
        void uploadFloat(std::string name, float value);
        void uploadVec2(std::string name, glm::vec2 vector);
        void uploadVec3(std::string name, glm::vec3 vector);
        void uploadVec4(std::string name, glm::vec4 vector);
        void uploadMat4(std::string name, glm::mat4 matrix);
        void uploadTexture(std::string name, int slot);
        void uploadIntArray(std::string name, int num, int* array);
        void uploadUnsignedIntArray(std::string name, int num, unsigned int* array);

};