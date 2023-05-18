#include <iostream>
#include "texture.hpp"

Texture::Texture(GLint internal, int width, int height, GLenum format, GLenum type) {

    // Store dimensions.
    this->width = width;
    this->height = height;

    // Generate texture on GPU
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, type, 0);

}

Texture::~Texture() {
    glDeleteTextures(1, &this->id);
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, this->id);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Texture::getId() {
    return this->id;
}

int Texture::getWidth() {
    return this->width;
}

int Texture::getHeight() {
    return this->height;
}