#ifndef DRAWABLEOBJECT_H_
#define DRAWABLEOBJECT_H_

#include <glad/glad.h>
#include <tiny_obj_loader.h>
#include "Window.h"
#include "loader.h"

constexpr GLuint POSITION = 0;
constexpr GLuint COLOR = 1;
constexpr GLuint NORMAL = 2;

class DrawableObject : public Drawable
{
public:
    DrawableObject() : _length(0), M(1.0f)
    {
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);

        glGenBuffers(1, &vertexBuffer);
        loadVertices(nullptr, 0);
        glBindVertexArray(vertexArray);

        glGenBuffers(1, &colorBuffer);
        loadColors(nullptr, 0);
        glBindVertexArray(vertexArray);

        glGenBuffers(1, &normalBuffer);
        loadNormals(nullptr, 0);
    }
    void loadVertices(const GLfloat *vertices, int length)
    {
        loadBuffers(vertices, length, POSITION, vertexBuffer, 3);
        _length = length;
    }
    void loadColors(const GLfloat *colors, int length)
    {
        loadBuffers(colors, length, COLOR, colorBuffer, 3);
    }
    void loadNormals(const GLfloat *normals, int length)
    {
        loadBuffers(normals, length, NORMAL, normalBuffer, 3);
    }
    void draw(Window *window) const override
    {
        window->shaderProgram.uniformMatrix4fv("M", glm::value_ptr(M));
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, _length);
        glBindVertexArray(0);
    }

protected:
    glm::mat4 M;

private:
    void loadBuffers(const GLfloat *data, int length, GLuint index, GLuint buffer, GLint size)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, length, data, GL_STATIC_DRAW);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(index);

    }
    GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint colorBuffer;
    GLuint normalBuffer;
    int _length;
};

class DummyModel : public DrawableObject
{
public:
    DummyModel()
    {
        std::vector<float> vertices, colors, normals;
        loadObjFile("BOSCH_WLG.obj", vertices, colors, normals);
        loadVertices(&vertices[0], vertices.size());
        loadColors(&colors[0], colors.size());
        loadNormals(&normals[0], normals.size());
    }
};

#endif /* !DRAWABLEOBJECT_H_ */
