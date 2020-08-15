#ifndef D30F1205_6249_4CE1_B72C_B4FBDC5DDD60
#define D30F1205_6249_4CE1_B72C_B4FBDC5DDD60

#include <ft2build.h>
#include FT_FREETYPE_H
#include "Gui.h"

class Text : public GuiObject
{
private:
    class Glyph : public GuiObject
    {
    public:
        static void init()
        {
            glyphShaderProgram.load("shaders/glyph_vert.glsl", "shaders/glyph_frag.glsl");
        }
        void load(Texture2d &&newTexture, float sizeX, float sizeY, float bearingX, float bearingY, float offsetValue)
        {
            texture = std::move(newTexture);
            size = glm::vec2(sizeX, sizeY);
            bearing = glm::vec2(bearingX, bearingY);
            offset = offsetValue;
            vertexArray.loadVerticesTexCoords({bearing.x, -(size.y - bearing.y) + size.y, 0.f,
                                               bearing.x, -(size.y - bearing.y), 0.f,
                                               bearing.x + size.x, -(size.y - bearing.y), 0.f,
                                               bearing.x, -(size.y - bearing.y) + size.y, 0.f,
                                               bearing.x + size.x, -(size.y - bearing.y), 0.f,
                                               bearing.x + size.x, -(size.y - bearing.y) + size.y, 0.f},
                                              {0.0f, 0.0f,
                                               0.0f, 1.0f,
                                               1.0f, 1.0f,
                                               0.0f, 0.0f,
                                               1.0f, 1.0f,
                                               1.0f, 0.0f});
        }
        Glyph()
            : texture(0), size(0.f, 0.f), bearing(0.f, 0.f), offset(0.f)
        {
            setPosition({0.f, 0.f});
        }
        void draw(Window *window) const override
        {
            DrawableObject::draw(window);
        }
        void setPosition(const glm::vec2 &newPosition) override
        {
            position = newPosition;
            M = glm::scale(glm::translate(glm::vec3(newPosition.x, newPosition.y, 0.f)), glm::vec3(scale / aspectRatio, scale, scale));
        }
        float getOffset() const { return scale * offset / aspectRatio / 64.f; }
        static void setAspectRatioAndScale(float newAspectRatio, float newScale)
        {
            aspectRatio = newAspectRatio;
            scale = newScale;
        }

    protected:
        const VertexArray &getVertexArray() const override { return vertexArray; }
        const ShaderProgram &getShaderProgram() const override { return glyphShaderProgram; }
        const Texture2d &getTexture0() const override { return texture; }
        bool getBlend() const override { return true; };
        bool getDepthMask() const override { return false; };

    private:
        friend class Text;
        static inline ShaderProgram glyphShaderProgram;
        Texture2d texture;
        VertexArray vertexArray;
        glm::vec2 size;
        glm::vec2 bearing;
        float offset;
        static inline float aspectRatio = 1.f;
        static inline float scale = 0.001f;
    };

public:
    static void init()
    {
        Glyph::init();
        FT_Library ft;
        if (FT_Init_FreeType(&ft))
            errorAndExit("FT_Init_FreeType");

        FT_Face face;
        if (FT_New_Face(ft, "PressStart2P-Regular.ttf", 0, &face))
            errorAndExit("FT_New_Face");

        FT_Set_Pixel_Sizes(face, 0, 32);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        wcharToGlyph.resize(127);
        std::vector<unsigned char> memory;

        for (char character = 0; character < 127; character++)
        {
            if (FT_Load_Char(face, character, FT_LOAD_RENDER))
                errorAndExit("FT_Load_Char");

            Texture2d texture;
            texture.load(face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, face->glyph->bitmap.buffer);

            wcharToGlyph[character].load(std::move(texture),
                                         face->glyph->bitmap.width, face->glyph->bitmap.rows,
                                         face->glyph->bitmap_left, face->glyph->bitmap_top,
                                         face->glyph->advance.x);
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
    Text()
    {
    }
    Text(const std::string &text)
        : string(text)
    {
    }
    void setText(const std::string &text)
    {
        string = text;
    }
    void draw(Window *window) const override
    {
        ShaderProgram::setColor(color);
        glm::vec2 temp(0.f, 0.f);
        for (const auto &character : string)
        {
            wcharToGlyph[character].setPosition(position + temp);
            wcharToGlyph[character].setColor(color);
            wcharToGlyph[character].draw(window);
            temp.x += (wcharToGlyph[character].getOffset());
        }
    }
    static void setAspectRatioAndScale(float newAspectRatio, float newScale)
    {
        Glyph::setAspectRatioAndScale(newAspectRatio, newScale);
    }

private:
    std::string string;
    static inline std::vector<Glyph> wcharToGlyph;
};

#endif /* D30F1205_6249_4CE1_B72C_B4FBDC5DDD60 */