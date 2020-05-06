#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <cassert>
#include <vector>
#include <glad/glad.h>
#include <lodepng.h>
#include "error.h"

class Texture
    {
    public:
        Texture() : texture(0), loaded(false) {}
        ~Texture()
        {
            if (loaded)
            {
                glDeleteTextures(1, &texture);
            }
        }
        void load(const std::string &imageFilename)
        {
            assert(loaded == false);
            std::vector<unsigned char> bytes;
            unsigned int width, height, error;
            error = lodepng::decode(bytes, width, height, imageFilename);
            if (error != 0)
                errorAndExit("Texture ", imageFilename, " not loaded");
            glActiveTexture(GL_TEXTURE0);
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            loaded = true;
        }
        GLuint getTextureId() const { return texture; }

    private:
        bool loaded;
        GLuint texture;
    };

#endif /* !TEXTURE_H_ */
