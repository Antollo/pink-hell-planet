#ifndef GUI_H_
#define GUI_H_

#include "DrawableObject.h"

class GuiObject : public DrawableObject
{
public:
    static void init()
    {
        guiShaderProgram.load("shaders/gui_vert.glsl", "shaders/gui_frag.glsl");
    }

    GuiObject()
        : color(1.f, 1.f, 1.f, 1.f), position(0.f, 0.f), positionOffset(0.f, 0.f)
    {
        setPosition();
    }
    virtual ~GuiObject() {}

    void setPosition(const glm::vec2 &newPosition = glm::vec2(0.f, 0.f))
    {
        position = newPosition;
        updateMatrixM();
    }
    void setPositionOffset(const glm::vec2 &newPositionOffset = glm::vec2(0.f, 0.f))
    {
        positionOffset = newPositionOffset;
        updateMatrixM();
    }
    virtual void setColor(const glm::vec4 &newColor)
    {
        color = newColor;
    }
    void draw(Window *window) const override
    {
        ShaderProgram::setColor(color);
        DrawableObject::draw(window);
    }
    static void setAspectRatioAndScale(float newAspectRatio, float newScale)
    {
        aspectRatio = newAspectRatio;
        scale = newScale;
    }

protected:
    float getAlpha() const override
    {
        return color.a;
    }
    Window::ProjectionMode getProjectionMode() const override
    {
        return Window::ProjectionMode::ortho;
    };
    const ShaderProgram &getShaderProgram() const override
    {
        return guiShaderProgram;
    }
    static float getAspectRatio()
    {
        return aspectRatio;
    }
    static float getScale()
    {
        return scale;
    }
    glm::vec4 color;
    glm::vec2 position;
    glm::vec2 positionOffset;

private:
    void updateMatrixM()
    {
        M = glm::scale(
            glm::translate(glm::vec3(position.x + positionOffset.x * getScale(), position.y + positionOffset.y * getScale() * getAspectRatio(), 0.f)),
            glm::vec3(getScale(), getScale() * getAspectRatio(), getScale()));
    }
    static inline ShaderProgram guiShaderProgram;
    static inline float aspectRatio = 1.f;
    static inline float scale = 0.001f;
};

#endif /* GUI_H_ */
