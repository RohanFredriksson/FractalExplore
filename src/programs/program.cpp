#include "programs/program.hpp"

std::string ShaderProgram::getName() {
    return this->name;
}

std::string ShaderProgram::getType() {
    return this->type;
}

Shader* ShaderProgram::getShader() {
    if (this->program == nullptr) {this->compile();}
    return this->program;
}

void ShaderProgram::setName(std::string name) {
    this->name = name;
} 

void ShaderProgram::setType(std::string type) {
    this->type = type;
}

std::string ShaderProgram::vertex() {
    return "";
}

std::string ShaderProgram::fragment() {
    return "";
}

void ShaderProgram::compile() {
    if (this->program != nullptr) {delete this->program;}
    this->program = new Shader(this->vertex(), this->fragment());
}

void ShaderProgram::upload() {

}

void ShaderProgram::imgui() {

}