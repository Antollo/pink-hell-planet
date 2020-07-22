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
        : vertexArray(0), vertexBuffer(0), colorBuffer(0), normalBuffer(0), texCoordBuffer(0), tangentBuffer(0), bitangentBuffer(0), length(0), loaded(false) {}
    ~VertexArray()
    {
        if (loaded)
        {
            if (vertexBuffer != 0)
                glDeleteBuffers(1, &vertexBuffer);
            if (colorBuffer != 0)
                glDeleteBuffers(1, &colorBuffer);
            if (normalBuffer != 0)
                glDeleteBuffers(1, &normalBuffer);
            if (texCoordBuffer != 0)
                glDeleteBuffers(1, &texCoordBuffer);
            if (tangentBuffer != 0)
                glDeleteBuffers(1, &tangentBuffer);
            if (bitangentBuffer != 0)
                glDeleteBuffers(1, &bitangentBuffer);
            glDeleteVertexArrays(1, &vertexArray);
        }
    }
    std::vector<float> load(const char *modelFilename, const float& scale = 1.f)
    {
        std::vector<float> vertices, colors, normals, texCoords;
        loadObjFile(modelFilename, vertices, colors, normals, texCoords, scale);

        std::vector<float> tangents(vertices.size()), bitangents(vertices.size());
        computeTangentAndBitangentVectors(vertices, normals, texCoords, tangents, bitangents);

        if (vertexArray == 0)
            createVertexArray(vertices, colors, normals, texCoords, tangents, bitangents);
        else
            updateVertexArray(vertices, colors, normals, texCoords, tangents, bitangents);
        return vertices;
    }
    const std::vector<float> &load(const std::vector<float> &vertices)
    {
        if (vertexArray == 0)
            createVertexArray(vertices);
        else
            updateVertexArray(vertices);
        return vertices;
    }
    const std::vector<float> &load(const std::vector<float> &vertices, const std::vector<float> &colors, const std::vector<float> &normals, const std::vector<float> &texCoords)
    {
        std::vector<float> tangents(vertices.size()), bitangents(vertices.size());
        computeTangentAndBitangentVectors(vertices, normals, texCoords, tangents, bitangents);

        if (vertexArray == 0)
            createVertexArray(vertices, colors, normals, texCoords, tangents, bitangents);
        else
            updateVertexArray(vertices, colors, normals, texCoords, tangents, bitangents);
        return vertices;
    }
    const std::vector<float> &load(const std::vector<float> &vertices, const std::vector<float> &normals, const std::vector<float> &texCoords, const std::vector<float> &tangents, const std::vector<float> &bitangents)
    {
        if (vertexArray == 0)
            createVertexArray(vertices, {}, normals, texCoords, tangents, bitangents);
        else
            updateVertexArray(vertices, {}, normals, texCoords, tangents, bitangents);
        return vertices;
    }
    GLuint getVertexArrayId() const { return vertexArray; }
    GLsizei getLength() const { return length; }

    static inline glm::vec3 computeTangent(const glm::vec3& e1, const glm::vec3& e2, const glm::vec2& t1, const glm::vec2& t2)
    {
        float r = 1.0 / (t1.x * t2.y - t1.y * t2.x);
        return glm::normalize((e1 * t2.y - e2 * t1.y) * r);
    }

    static inline glm::vec3 computeBitangent(const glm::vec3& tangent, const glm::vec3& normal)
    {
        return glm::normalize(glm::cross(tangent, normal));
    }

private:
    enum class bufferIndex
    {
        POSITION = 0,
        COLOR = 1,
        NORMAL = 2,
        TEXCOORD = 3,
        TANGENT = 4,
        BITANGENT = 5
    };

    GLuint vertexArray;
    GLuint vertexBuffer;
    GLuint colorBuffer;
    GLuint normalBuffer;
    GLuint texCoordBuffer;
    GLuint tangentBuffer;
    GLuint bitangentBuffer;
    GLsizei length;
    bool loaded;

    void computeTangentAndBitangentVectors(const std::vector<float> &vertices, const std::vector<float> &normals, const std::vector<float> &texCoords, std::vector<float> &tangents, std::vector<float> &bitangents) const
    {
        for (int i = 0; i < texCoords.size() / 6; i++)
        {
            glm::vec3 e1(glm::vec3(vertices[9 * i + 3], vertices[9 * i + 4], vertices[9 * i + 5]) - glm::vec3(vertices[9 * i], vertices[9 * i + 1], vertices[9 * i + 2]));
            glm::vec3 e2(glm::vec3(vertices[9 * i + 6], vertices[9 * i + 7], vertices[9 * i + 8]) - glm::vec3(vertices[9 * i], vertices[9 * i + 1], vertices[9 * i + 2]));

            glm::vec2 t1(glm::vec2(texCoords[6 * i + 2], texCoords[6 * i + 3]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));
	        glm::vec2 t2(glm::vec2(texCoords[6 * i + 4], texCoords[6 * i + 5]) - glm::vec2(texCoords[6 * i], texCoords[6 * i + 1]));

            glm::vec3 tangent(computeTangent(e1, e2, t1, t2));

            glm::vec3 bitangentA(computeBitangent(tangent, glm::vec3(normals[9 * i], normals[9 * i + 1], normals[9 * i + 2])));
            glm::vec3 bitangentB(computeBitangent(tangent, glm::vec3(normals[9 * i + 3], normals[9 * i + 4], normals[9 * i + 5])));
            glm::vec3 bitangentC(computeBitangent(tangent, glm::vec3(normals[9 * i + 6], normals[9 * i + 7], normals[9 * i + 8])));

            tangents[9 * i] = tangent.x, tangents[9 * i + 1] = tangent.y, tangents[9 * i + 2] = tangent.z;
            tangents[9 * i + 3] = tangent.x, tangents[9 * i + 4] = tangent.y, tangents[9 * i + 5] = tangent.z;
            tangents[9 * i + 6] = tangent.x, tangents[9 * i + 7] = tangent.y, tangents[9 * i + 8] = tangent.z;

            bitangents[9 * i] = bitangentA.x, bitangents[9 * i + 1] = bitangentA.y, bitangents[9 * i + 2] = bitangentA.z;
            bitangents[9 * i + 3] = bitangentB.x, bitangents[9 * i + 4] = bitangentB.y, bitangents[9 * i + 5] = bitangentB.z;
            bitangents[9 * i + 6] = bitangentC.x, bitangents[9 * i + 7] = bitangentC.y, bitangents[9 * i + 8] = bitangentC.z;
        }
    }

    void createVertexArray(const std::vector<float> &vertices, const std::vector<float> &colors, const std::vector<float> &normals, const std::vector<float> &texCoords, const std::vector<float> &tangents, const std::vector<float> &bitangents)
    {
        assert(loaded == false);
        //assert(vertices.size() == colors.size());
        assert(vertices.size() == normals.size());
        assert(vertices.size() * 2 == texCoords.size() * 3);
        assert(vertices.size() == tangents.size());
        assert(vertices.size() == bitangents.size());
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        loadBuffer(vertices.data(), vertices.size(), bufferIndex::POSITION, vertexBuffer);
        loadBuffer(colors.data(), colors.size(), bufferIndex::COLOR, colorBuffer);
        loadBuffer(normals.data(), normals.size(), bufferIndex::NORMAL, normalBuffer);
        loadBuffer(texCoords.data(), texCoords.size(), bufferIndex::TEXCOORD, texCoordBuffer, 2);
        loadBuffer(tangents.data(), tangents.size(), bufferIndex::TANGENT, tangentBuffer);
        loadBuffer(bitangents.data(), bitangents.size(), bufferIndex::BITANGENT, bitangentBuffer);
        glBindVertexArray(0);
        length = vertices.size();
        loaded = true;
    }
    void updateVertexArray(const std::vector<float> &vertices, const std::vector<float> &colors, const std::vector<float> &normals, const std::vector<float> &texCoords, const std::vector<float> &tangents, const std::vector<float> &bitangents)
    {
        assert(loaded == true);
        //assert(vertices.size() == colors.size());
        assert(vertices.size() == normals.size());
        assert(vertices.size() * 2 == texCoords.size() * 3);
        assert(vertices.size() == tangents.size());
        assert(vertices.size() == bitangents.size());
        glBindVertexArray(vertexArray);
        updateBuffer(vertices.data(), vertices.size(), bufferIndex::POSITION, vertexBuffer);
        updateBuffer(colors.data(), colors.size(), bufferIndex::COLOR, colorBuffer);
        updateBuffer(normals.data(), normals.size(), bufferIndex::NORMAL, normalBuffer);
        updateBuffer(texCoords.data(), texCoords.size(), bufferIndex::TEXCOORD, texCoordBuffer, 2);
        updateBuffer(tangents.data(), tangents.size(), bufferIndex::TANGENT, tangentBuffer);
        updateBuffer(bitangents.data(), bitangents.size(), bufferIndex::BITANGENT, bitangentBuffer);
        glBindVertexArray(0);
        length = vertices.size();
    }
    void createVertexArray(const std::vector<float> &vertices)
    {
        assert(loaded == false);
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        loadBuffer(vertices.data(), vertices.size(), bufferIndex::POSITION, vertexBuffer);
        glBindVertexArray(0);
        length = vertices.size();
        loaded = true;
    }
    void updateVertexArray(const std::vector<float> &vertices)
    {
        assert(loaded == true);
        glBindVertexArray(vertexArray);
        updateBuffer(vertices.data(), vertices.size(), bufferIndex::POSITION, vertexBuffer);
        glBindVertexArray(0);
        length = vertices.size();
    }
    void loadBuffer(const GLfloat *data, int length, bufferIndex index, GLuint& buffer, GLint size = 3)
    {
        glGenBuffers(1, &buffer);
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
