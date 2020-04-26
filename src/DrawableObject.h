#ifndef DRAWABLEOBJECT_H_
#define DRAWABLEOBJECT_H_

#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include "Window.h"
#include "loader.h"

class DrawableObject : public Drawable
{
public:
    enum class bufferIndex
    {
        POSITION = 0,
        COLOR = 1,
        NORMAL = 2
    };
    static void init() { defaultShaderProgram.load("shaders/vert.glsl", "shaders/frag.glsl"); }
    DrawableObject() : M(1.0f) {}
    void draw(Window *window) const override
    {
        getShaderProgram().use();
        getShaderProgram().setUniformMatrix4fv("M", M);
        glBindVertexArray(getVertexArray());
        glDrawArrays(GL_TRIANGLES, 0, getVertexArrayLength());
        glBindVertexArray(0);
    }

protected:
    glm::mat4 M;
    static inline ShaderProgram defaultShaderProgram;

    virtual GLuint getVertexArray() const = 0;
    virtual int getVertexArrayLength() const = 0;
    virtual const ShaderProgram &getShaderProgram() const { return defaultShaderProgram; };

    class VertexArray
    {
    public:
        VertexArray() : vertexArray(0), vertexBuffer(0), colorBuffer(0), normalBuffer(0), length(0) {}
        void load(const std::string &modelFilename)
        {
            std::vector<float> vertices, colors, normals;
            loadObjFile(modelFilename, vertices, colors, normals);
            createVertexArray(vertices, colors, normals);
        }
        void load(std::vector<float> &vertices, std::vector<float> &colors, std::vector<float> &normals)
        {
            createVertexArray(vertices, colors, normals);
        }

        GLuint vertexArray;
        GLuint vertexBuffer;
        GLuint colorBuffer;
        GLuint normalBuffer;
        GLsizei length;

    private:
        void createVertexArray(std::vector<float> &vertices, std::vector<float> &colors, std::vector<float> &normals)
        {
            glGenVertexArrays(1, &vertexArray);
            glBindVertexArray(vertexArray);
            glGenBuffers(1, &vertexBuffer);
            loadBuffer(&vertices[0], vertices.size(), bufferIndex::POSITION, vertexBuffer);
            glGenBuffers(1, &colorBuffer);
            loadBuffer(&colors[0], colors.size(), bufferIndex::COLOR, colorBuffer);
            glGenBuffers(1, &normalBuffer);
            loadBuffer(&normals[0], normals.size(), bufferIndex::NORMAL, normalBuffer);
            glBindVertexArray(0);
            length = vertices.size();
        }
        void loadBuffer(const GLfloat *data, int length, bufferIndex index, GLuint buffer, GLint size = 3)
        {
            glBindBuffer(GL_ARRAY_BUFFER, buffer);
            glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);
            glVertexAttribPointer(static_cast<GLuint>(index), size, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(static_cast<GLuint>(index));
        }
    };
};

class DummyModel : public DrawableObject
{
public:
    static void init()
    {
        shaderProgram.load("shaders/vert.glsl", "shaders/frag.glsl");
        vertexArray.load("BOSCH_WLG.obj");
    }

protected:
    virtual GLuint getVertexArray() const override { return vertexArray.vertexArray; }
    virtual int getVertexArrayLength() const override { return vertexArray.length; }
    virtual const ShaderProgram &getShaderProgram() const override  { return shaderProgram; }

private:
    static inline VertexArray vertexArray;
    static inline ShaderProgram shaderProgram;
};

#endif /* !DRAWABLEOBJECT_H_ */
