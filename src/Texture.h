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
    virtual ~Texture()
    {
        if (loaded)
        {
            glDeleteTextures(1, &texture);
        }
    }
    virtual void load(const std::string &imageFilename, bool srgb, bool alpha, bool anisotropicFiltering) = 0;

    GLuint getTextureId() const { return texture; }

protected:
    bool loaded;
    GLuint texture;
};

class Texture2d : public Texture
{
public:
    void load(const std::string &imageFilename, bool srgb = false, bool alpha = false, bool anisotropicFiltering = true) override
    {
        assert(loaded == false);
        std::vector<unsigned char> bytes;
        unsigned int width, height, error;
        error = lodepng::decode(bytes, width, height, imageFilename + ".png");
        if (error != 0)
            errorAndExit("Texture ", imageFilename + ".png", " not loaded");
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, alpha ? (srgb ? GL_SRGB_ALPHA : GL_RGBA) : (srgb ? GL_SRGB : GL_RGB), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());

        if (anisotropicFiltering)
        {
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            float maxAnisotropy;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, glm::min(maxAnisotropy, 8.f));
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        loaded = true;
    }
};

class Texture3d : public Texture
{
public:
    void load(const std::string &imageFilename, bool srgb = false, bool alpha = false, bool anisotropicFiltering = true) override
    {
        assert(loaded == false);

        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        /*
        0 right
        1 left
        2 top
        3 bottom
        4 front
        5 back
        */

        for (unsigned int i = 0; i < 6; i++)
        {
            std::vector<unsigned char> bytes;
            unsigned int width, height, error;

            auto ithImageFilename = imageFilename + "/" + std::to_string(i) + ".png";

            error = lodepng::decode(bytes, width, height, ithImageFilename);
            if (error != 0)
                errorAndExit("Texture ", ithImageFilename, " not loaded");

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, alpha ? (srgb ? GL_SRGB_ALPHA : GL_RGBA) : (srgb ? GL_SRGB : GL_RGB), width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data());
        }

        if (anisotropicFiltering)
        {
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            float maxAnisotropy;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, glm::min(maxAnisotropy, 8.f));
        }
        else
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        loaded = true;
    }
};

#endif /* !TEXTURE_H_ */
