#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>
#include <utility>
#include <string>

#include "graphics/shader.hpp"

class FractalFactory {

    private:
        std::unordered_map<std::string, std::pair<const char*, const char*>> fractals;

    public:

        static FractalFactory& get() {
            static FractalFactory instance;
            return instance;
        }

        void add(std::string name, const char* vertex, const char* fragment) {
            this->fractals.insert({name, std::make_pair(vertex, fragment)});
        }

        Shader* create(std::string name, int precision) {

            const auto it = this->fractals.find(name);
            if (it == this->fractals.end()) {return nullptr;}
        
            std::string v(it->second.first);
            std::string f(it->second.second);
            
            std::string search = "${PRECISION}";
            size_t position = f.find(search);
            if (position != std::string::npos) {f.replace(position, search.length(), std::to_string(precision));}

            return new Shader(v, f);
          
        }
        
        void list() {
            for (const auto &creator : this->fractals) {
                std::cout << creator.first << '\n';
            }
        }

};

class Fractal {

    public:

        explicit Fractal(std::string name, const char* vertex, const char* fragment) {
            FractalFactory::get().add(name, vertex, fragment);
        }

};

#define UNIQUE_VARIABLE_NAME() CONCATENATE(V, __COUNTER__)
#define CONCATENATE_IMPL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_IMPL(x, y)

#define FRACTAL_FACTORY FractalFactory::get()
#define REGISTER_FRACTAL(Class, v, f) namespace{Fractal UNIQUE_VARIABLE_NAME()(#Class, v, f);}