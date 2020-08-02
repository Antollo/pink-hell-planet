#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <glm/gtc/random.hpp>
#include "Clock.h"
#include "DrawableObject.h"

class ParticleSystem : public DrawableObject
{
public:
    static void init()
    {
        particleSystemDrawShaderProgram.load("shaders/particleSystem_vert.glsl",
                                             "shaders/particleSystem_geom.glsl",
                                             "shaders/particleSystem_frag.glsl");

        texture.load("firefly_DIFF", true, true);
    }

    void draw(Window *window) const override
    {
        ShaderProgram::setTime(v.getTime());
        getShaderProgram().use();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTexture0().getTextureId());
        getShaderProgram().setUniform1i("tex0", 0);
        getShaderProgram().validate();
        glBindVertexArray(v.getVertexArray().getVertexArrayId());
        glDrawArrays(GL_POINTS, 0, v.getVertexArray().getLength(GL_POINTS));
        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
    void update()
    {
        v.generate({20.f, 15.f, 10.f});
    }

protected:
    const VertexArray &getVertexArray() const override { return defaultVertexArray; }
    const ShaderProgram &getShaderProgram() const override { return particleSystemDrawShaderProgram; }
    const Texture2d &getTexture0() const override { return texture; }

private:
    class ParticleGroup
    {
    public:
        void generate(const glm::vec3& center)
        {
            if (clock.getTime() < 6.f) // Just for testing
                return;
            std::vector<float> position(verticesCount * 3);
            std::vector<float> velocity(verticesCount * 3);
            std::vector<float> other(verticesCount * 3);
            for (int i = 0; i < verticesCount; i++)
            {
                glm::vec3 currentPosition = center + glm::ballRand(0.01f);
                glm::vec3 currentVelocity = glm::ballRand(10.f);

                position[i * 3] = currentPosition.x;
                position[i * 3 + 1] = currentPosition.y;
                velocity[i * 3 + 2] = currentPosition.z;

                velocity[i * 3] = currentVelocity.x;
                velocity[i * 3 + 1] = currentVelocity.y;
                velocity[i * 3 + 2] = currentVelocity.z;
                
                other[i * 3] = glm::gaussRand(0.2f, 0.1f);
                other[i * 3 + 1] = glm::gaussRand(3.f, 1.f);
                other[i * 3 + 2] = 1.f;
            }
            clock.reset();
            vertexArray.load(position, velocity, other);
        }
        const VertexArray &getVertexArray() const { return vertexArray; }
        float getTime() const { return clock.getTime(); }
       
    private:
        static constexpr int verticesCount = 20000;
        VertexArray vertexArray;
        Clock clock;
    };

    static inline ShaderProgram particleSystemDrawShaderProgram;
    static inline Texture2d texture;
    ParticleGroup v;
};

#endif /* PARTICLESYSTEM_H */
