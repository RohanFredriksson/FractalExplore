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

class ShaderProgram {

    protected:

        std::string name = "";
        std::string type = "";
        Shader* program = nullptr;

    public:

        std::string getName() {
            return this->name;
        }

        std::string getType() {
            return this->type;
        }

        Shader* getShader() {
            if (this->program == nullptr) {this->compile();}
            return this->program;
        }

        void setName(std::string name) {
            this->name = name;
        } 

        void setType(std::string type) {
            this->type = type;
        }
        
        virtual std::string vertex() {
            return "";
        }

        virtual std::string fragment() {
            return "";
        }

        virtual void compile() {
            if (this->program != nullptr) {delete this->program;}
            this->program = new Shader(this->vertex(), this->fragment());
        }

        virtual void upload() {

        }

        virtual void imgui() {

        }
        
};

class ShaderProgramPool {

    private:
        std::unordered_map<std::string, std::unordered_map<std::string, ShaderProgram*>> programs;

    public:

        static ShaderProgramPool& get() {
            static ShaderProgramPool instance;
            return instance;
        }

        void add(std::string type, std::string name, ShaderProgram* program) {

            program->setName(name);
            program->setType(type);

            const auto search = this->programs.find(type);
            if (search == this->programs.end()) {
                std::unordered_map<std::string, ShaderProgram*> pool;
                pool.insert({name, program});
                this->programs.insert({type, pool});
            } else {
                search->second.insert({name, program});
            }

        }

        ShaderProgram* get(std::string type, std::string name) {

            const auto search = this->programs.find(type);
            if (search == this->programs.end()) {return nullptr;}

            const auto it = search->second.find(name);
            if (it == search->second.end()) {return nullptr;}

            return it->second;

        }

        std::vector<std::string> names(std::string type) {

            std::vector<std::string> result;
            const auto search = this->programs.find(type);
            if (search == this->programs.end()) {return result;}
            for (const auto &it : search->second) {
                result.push_back(it.first);
            }

            return result;

        }

        std::vector<ShaderProgram*> shaders(std::string type) {

            std::vector<ShaderProgram*> result;
            const auto search = this->programs.find(type);
            if (search == this->programs.end()) {return result;}
            for (const auto &it : search->second) {
                result.push_back(it.second);
            }

            return result;

        }

};

class ShaderProgramCreator {

    public:

        explicit ShaderProgramCreator(std::string type, std::string name, ShaderProgram* program) {
            ShaderProgramPool::get().add(type, name, program);
        }

};

#define SHADER_PROGRAM_POOL ShaderProgramPool::get()
#define REGISTER_SHADER_PROGRAM(Type, Class) namespace{ShaderProgramCreator UNIQUE_VARIABLE_NAME()(#Type, #Class, new Class());}