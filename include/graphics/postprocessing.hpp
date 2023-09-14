#pragma once

#include <unordered_map>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
#include <string>

#include "util.hpp"
#include "graphics/shader.hpp"

class PostProcessing {

    private:
        std::unordered_map<std::string, std::pair<const char*, const char*>> shaders;

    public:

        static PostProcessing& getInstance() {
            static PostProcessing instance;
            return instance;
        }

        void add(std::string name, const char* vertex, const char* fragment) {
            this->shaders.insert({name, std::make_pair(vertex, fragment)});
        }

        Shader* get(std::string name) {

            const auto it = this->shaders.find(name);
            if (it == this->shaders.end()) {return nullptr;}
        
            std::string v(it->second.first);
            std::string f(it->second.second);

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

class PostProcessingLoader {

    public:

        explicit PostProcessingLoader(std::string name, const char* vertex, const char* fragment) {
            PostProcessing::getInstance().add(name, vertex, fragment);
        }

};

#define POSTPROCESSING PostProcessing::getInstance()
#define REGISTER_POSTPROCESSING(n, v, f) namespace{PostProcessingLoader UNIQUE_VARIABLE_NAME()(#n, v, f);}