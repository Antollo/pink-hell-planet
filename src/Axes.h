#ifndef AXES_H_
#define AXES_H_

#include "DrawableObject.h"

class Axes : public DrawableObject
{
public:
    static void init()
    {
        std::vector<float> vertices{
            0.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            0.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 0.f,
            0.f, 0.f, 1.f};
        std::vector<float> colors{
            1.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 1.f,
            0.f, 0.f, 1.f};
        std::vector<float> normals{
            1.f, 0.f, 0.f,
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 1.f,
            0.f, 0.f, 1.f};
        std::vector<float> texCoords{
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f,
            0.f, 0.f};
        vertexArray.load(vertices, colors, normals, texCoords);
        simpleShaderProgram.load("shaders/simple_vert.glsl", "shaders/simple_frag.glsl");
    }

    void draw(Window *window) const override
    {
        float lineWidth;
        glGetFloatv(GL_LINE_WIDTH, &lineWidth);
        glLineWidth(8.f);
        getShaderProgram().use();
        getShaderProgram().setUniformMatrix4fv("M", M);
        glBindVertexArray(getVertexArray().getVertexArrayId());
        glDrawArrays(GL_LINES, 0, getVertexArray().getLength());
        glBindVertexArray(0);
        glLineWidth(lineWidth);
    }
    void setMatrixM(const glm::mat4 &matrix) { M = matrix; }

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    const ShaderProgram &getShaderProgram() const override { return simpleShaderProgram; }

private:
    static inline VertexArray vertexArray;
    static inline ShaderProgram simpleShaderProgram;
};

#endif /* !AXES_H_ */