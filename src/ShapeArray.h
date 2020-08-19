#ifndef F888C33C_0DD5_4C47_81B9_E7F0435E558E
#define F888C33C_0DD5_4C47_81B9_E7F0435E558E

#include "Gui.h"

class ShapeArray : public GuiObject
{
public:
    enum class Type
    {
        lines = GL_LINES,
        points = GL_POINTS,
        triangles = GL_TRIANGLES
    };
    ShapeArray(Type newType = Type::points)
        : type(newType)
    {
    }
    void draw(Window *window) const override
    {
        glLineWidth(x);
        glPointSize(x);
        GuiObject::draw(window);
    }
    void setWidth(float newWidth)
    {
        x = newWidth;
    }
    void setRadius(float newRadius)
    {
        x = newRadius;
    }
    void setType(Type newType)
    {
        type = newType;
    }
    template <class Iterator>
    void insert(std::vector<glm::vec2>::iterator it, Iterator first, Iterator last)
    {
        vertices.insert(it, first, last);
        loadVertexArray();
    }
    void insert(std::vector<glm::vec2>::iterator it, std::initializer_list<glm::vec2> initializerList)
    {
        vertices.insert(it, initializerList);
        loadVertexArray();
    }
    void insert(std::vector<glm::vec2>::iterator it, const glm::vec2 &value)
    {
        vertices.insert(it, value);
        loadVertexArray();
    }
    void erase(std::vector<glm::vec2>::iterator it)
    {
        vertices.erase(it);
        loadVertexArray();
    }
    void erase(std::vector<glm::vec2>::iterator first, std::vector<glm::vec2>::iterator last)
    {
        vertices.erase(first, last);
        loadVertexArray();
    }
    size_t size() const
    {
        return vertices.size();
    }
    std::vector<glm::vec2>::iterator begin()
    {
        return vertices.begin();
    }
    std::vector<glm::vec2>::iterator end()
    {
        return vertices.end();
    }

protected:
    const VertexArray &getVertexArray() const override { return vertexArray; }
    GLenum getMode() const override { return static_cast<GLenum>(type); };

private:
    void loadVertexArray()
    {
        std::vector<float> temp(vertices.size() * 3, 0.f);
        for (size_t i = 0; i < vertices.size(); i++)
        {
            temp[3 * i] = vertices[i].x;
            temp[3 * i + 1] = vertices[i].y;
        }
        vertexArray.loadVertices(temp);
    }
    std::vector<glm::vec2> vertices;
    float x;
    Type type;
    VertexArray vertexArray;
};

#endif /* F888C33C_0DD5_4C47_81B9_E7F0435E558E */
