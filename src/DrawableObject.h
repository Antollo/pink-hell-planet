#ifndef DRAWABLEOBJECT_H_
#define DRAWABLEOBJECT_H_

#include <cassert>
#include <glad/glad.h>
#include "Window.h"
#include "Texture.h"
#include "VertexArray.h"

class DrawableObject : public Drawable
{
public:
    static void init()
    {
        defaultTexture3d.load("skybox", true);
    }

    DrawableObject() : M(1.0f) {}
    virtual ~DrawableObject() {}

    virtual void update(float delta) {}

    void draw(Window *window) const override
    {
        getShaderProgram().use();
        getShaderProgram().setUniformMatrix4fv("M", M);
        if (getTexture0().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, getTexture0().getTextureId());
            getShaderProgram().setUniform1i("tex0", 0);
        }
        if (getTexture1().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, getTexture1().getTextureId());
            getShaderProgram().setUniform1i("tex1", 1);
        }
        if (getTexture2().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, getTexture2().getTextureId());
            getShaderProgram().setUniform1i("tex2", 2);
        }
        if (getTexture3().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, getTexture3().getTextureId());
            getShaderProgram().setUniform1i("tex3", 3);
        }
        if (getTexture3d().getTextureId() != 0)
        {
            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_CUBE_MAP, getTexture3d().getTextureId());
            getShaderProgram().setUniform1i("cube", 4);
        }
        if (getAlpha() != 1)
            glEnable(GL_BLEND);
        else if (getBlend())
            glEnable(GL_BLEND);
        getShaderProgram().setAlpha(getAlpha());
        if (!getDepthMask())
            glDepthMask(GL_FALSE);
        window->setProjectionMode(getProjectionMode());
        getShaderProgram().validate();
        glBindVertexArray(getVertexArray().getVertexArrayId());
        glDrawArrays(getMode(), 0, getVertexArray().getLength());
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        glBindVertexArray(0);
    }

    bool isAlive()
    {
        return alive;
    }

    void kill()
    {
        alive = false;
    }

    float getAlpha() const { return alpha; }
    const glm::mat4 &getM() const { return M; }

protected:
    glm::mat4 M;
    bool alive = true;

    virtual const VertexArray &getVertexArray() const { return defaultVertexArray; }
    virtual const ShaderProgram &getShaderProgram() const { return defaultShaderProgram; }
    virtual const Texture2d &getTexture0() const { return defaultTexture; }
    virtual const Texture2d &getTexture1() const { return defaultTexture; }
    virtual const Texture2d &getTexture2() const { return defaultTexture; }
    virtual const Texture2d &getTexture3() const { return defaultTexture; }
    virtual const Texture3d &getTexture3d() const { return defaultTexture3d; }
    virtual bool getBlend() const { return false; }
    virtual bool getDepthMask() const { return true; }
    virtual GLenum getMode() const { return GL_TRIANGLES; }
    virtual Window::ProjectionMode getProjectionMode() const { return Window::ProjectionMode::perspective; }

    void setAlpha(float x) { alpha = x; }
    void setM(const glm::mat4 & x) { M = x; }

    static inline Texture2d defaultTexture;
    static inline Texture3d defaultTexture3d;
    static inline VertexArray defaultVertexArray;
    static inline ShaderProgram defaultShaderProgram;

private:
    float alpha = 1.f;
};

class ManagedDrawableObject : public DrawableObject
{
public:
    using DrawableObject::setM;
    using DrawableObject::setAlpha;
};

#endif /* !DRAWABLEOBJECT_H_ */
