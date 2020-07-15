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
        Texture() : loaded(false), texture(0) {}
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
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            float maxAnisotropy;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, glm::min(maxAnisotropy, 4.f));

            loaded = true;
        }
        GLuint getTextureId() const { return texture; }

    private:
        bool loaded;
        GLuint texture;
    };

#endif /* !TEXTURE_H_ */
