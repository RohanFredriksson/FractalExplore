#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>

#include "util.hpp"
#include "graphics/shader.hpp"

class Fractal {

    private:
        std::unordered_map<std::string, std::pair<const char*, const char*>> shaders;

    public:

        static Fractal& getInstance() {
            static Fractal instance;
            return instance;
        }

        void add(std::string name, const char* vertex, const char* fragment) {
            this->shaders.insert({name, std::make_pair(vertex, fragment)});
        }

        Shader* get(std::string name, int precision) {

            const auto it = this->shaders.find(name);
            if (it == this->shaders.end()) {return nullptr;}
        
            std::string v(it->second.first);
            std::string f(it->second.second);
            
            std::string search = "${PRECISION}";
            size_t position = f.find(search);
            if (position != std::string::npos) {f.replace(position, search.length(), std::to_string(precision));}

            return new Shader(v, f);
          
        }
        
        std::vector<std::string> list() {
            
            std::vector<std::string> result;
            for (const auto &creator : this->shaders) {
                result.push_back(creator.first);
            }

            return result;

        }

};

class FractalLoader {

    public:

        explicit FractalLoader(std::string name, const char* vertex, const char* fragment) {
            Fractal::getInstance().add(name, vertex, fragment);
        }

};

#define FRACTAL Fractal::getInstance()
#define REGISTER_FRACTAL(n, v, f) namespace{FractalLoader UNIQUE_VARIABLE_NAME()(#n, v, f);}