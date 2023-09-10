#pragma once

#include <glad/glad.h>

#include "graphics/texture.hpp"

class Framebuffer {

    private:

        unsigned int fbo;
        Texture* texture;

    public:

        Framebuffer(GLint internal, int width, int height, GLenum format, GLenum type);
        ~Framebuffer();

        void bind();
        void unbind();

        Texture* getTexture();

};