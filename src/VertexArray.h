#ifndef VERTEXARRAY_H_
#define VERTEXARRAY_H_

#include <cassert>
#include <glad/glad.h>
#include "loader.h"
#include "error.h"

class VertexArray
{
public:
    VertexArray()
        : vertexArray(0), vertexBuffer(0), colorBuffer(0), normalBuffer(0), texCoordBuffer(0), length(0), loaded(false) {}
    ~VertexArray()
    {
        if (loaded)
        {
            glDeleteBuffers(1, &vertexBuffer);
            glDeleteBuffers(1, &colorBuffer);
            glDeleteBuffers(1, &normalBuffer);
            glDeleteBuffers(1, &texCoordBuffer);
            glDeleteVertexArrays(1, &vertexArray);
        }
    }
    std::vector<float> load(const char *modelFilename)
    {
        std::vector<float> vertices, colors, normals, texCoords;
        loadObjFile(modelFilename, vertices, colors, normals, texCoords);
        if (vertexArray == 0)
            createVertexArray(vertices, colors, normals, texCoords);
        else
            updateVertexArray(vertices, colors, normals, texCoords);
        return vertices;
    }
    const std::vector<float> &load(const std::vector<float> &vertices, const std::vector<float> &colors, const std::vector<float> &normals, const std::vector<float> &texCoords)
    {
        if (vertexArray == 0)
            createVertexArray(vertices, colors, normals, texCoords);
        else
            updateVertexArray(vertices, colors, normals, texCoords);
        return vertices;
    }
    GLuint getVertexArrayId() const { return vertexArray; }
    GLsizei getLength() const { return length; }

private:
    enum class bufferIndex
    {
        POSITION = 0,
        COLOR = 1,
        NORMAL = 2,
        TEXCOORD = 3
    };

    GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint colorBuffer;
    GLuint normalBuffer;
    GLuint texCoordBuffer;
    GLsizei length;
    bool loaded;
    void createVertexArray(const std::vector<float> &vertices, const std::vector<float> &colors, const std::vector<float> &normals, const std::vector<float> &texCoords)
    {
        assert(loaded == false);
        assert(vertices.size() == colors.size());
        assert(vertices.size() == normals.size());
        assert(vertices.size() * 2 == texCoords.size() * 3);
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glGenBuffers(1, &vertexBuffer);
        loadBuffer(vertices.data(), vertices.size(), bufferIndex::POSITION, vertexBuffer);
        glGenBuffers(1, &colorBuffer);
        loadBuffer(colors.data(), colors.size(), bufferIndex::COLOR, colorBuffer);
        glGenBuffers(1, &normalBuffer);
        loadBuffer(normals.data(), normals.size(), bufferIndex::NORMAL, normalBuffer);
        glGenBuffers(1, &texCoordBuffer);
        loadBuffer(texCoords.data(), texCoords.size(), bufferIndex::TEXCOORD, texCoordBuffer, 2);
        glBindVertexArray(0);
        length = vertices.size();
        loaded = true;
    }
    void updateVertexArray(const std::vector<float> &vertices, const std::vector<float> &colors, const std::vector<float> &normals, const std::vector<float> &texCoords)
    {
        assert(loaded == true);
        assert(vertices.size() == colors.size());
        assert(vertices.size() == normals.size());
        assert(vertices.size() * 2 == texCoords.size() * 3);
        glBindVertexArray(vertexArray);
        updateBuffer(vertices.data(), vertices.size(), bufferIndex::POSITION, vertexBuffer);
        updateBuffer(colors.data(), colors.size(), bufferIndex::COLOR, colorBuffer);
        updateBuffer(normals.data(), normals.size(), bufferIndex::NORMAL, normalBuffer);
        updateBuffer(texCoords.data(), texCoords.size(), bufferIndex::TEXCOORD, texCoordBuffer, 2);
        glBindVertexArray(0);
        length = vertices.size();
    }
    void loadBuffer(const GLfloat *data, int length, bufferIndex index, GLuint buffer, GLint size = 3)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, length * sizeof(GLfloat), data, GL_STATIC_DRAW);
        glVertexAttribPointer(static_cast<GLuint>(index), size, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(static_cast<GLuint>(index));
    }
    void updateBuffer(const GLfloat *data, int length, bufferIndex index, GLuint buffer, GLint size = 3)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, length * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
    }
};

#endif /* !VERTEXARRAY_H_ */
