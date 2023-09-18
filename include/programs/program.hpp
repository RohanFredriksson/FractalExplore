#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "graphics/shader.hpp"

class ShaderProgram {

    protected:

        std::string name = "";
        std::string type = "";
        Shader* program = nullptr;

    public:

        std::string getName();
        std::string getType();
        Shader* getShader();

        void setName(std::string name);
        void setType(std::string type);
        
        virtual std::string vertex();
        virtual std::string fragment();
        virtual void compile();
        virtual void upload();
        virtual void imgui();
        
};

#include <iostream>

class ShaderProgramPool {

    private:
        std::unordered_map<std::string, std::unordered_map<std::string, ShaderProgram*>> programs;

    public:

        static ShaderProgramPool& get() {
            static ShaderProgramPool instance;
            return instance;
        }

        void add(std::string type, std::string name, ShaderProgram* program) {

            std::cout << "ADDED: " << type << ": " << name << "\n";

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