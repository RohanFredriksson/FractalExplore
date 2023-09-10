#pragma once

#include <glad/glad.h>

class Texture {

    private:

        unsigned int id;
        int width;
        int height;

    public:

        Texture(GLint internal, int width, int height, GLenum format, GLenum type);
        ~Texture();

        void bind();
        void unbind();
        
        unsigned int getId();
        int getWidth();
        int getHeight();

};