#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>
#include <array>
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
        getShaderProgram().use();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTexture0().getTextureId());
        getShaderProgram().setUniform1i("tex0", 0);
        getShaderProgram().validate();
        for (auto &particleGroup : particleGroups)
        {
            if (particleGroup.getTime() > 6.f)
                continue;
            ShaderProgram::setTime(particleGroup.getTime());
            glBindVertexArray(particleGroup.getVertexArray().getVertexArrayId());
            glDrawArrays(GL_POINTS, 0, particleGroup.getVertexArray().getLength(GL_POINTS));
        }
        glBindVertexArray(0);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        for (auto &particleGroup : particleGroups)
            particleGroup.drawAxes(window);
    }
    void update(float time60Pi)
    {
        globalTime = time60Pi;
    }
    void generate(const glm::vec3 &center)
    {
        static int i = 0;
        particleGroups[i].generate(center, globalTime);
        i++;
        i %= particleGroupCount;
    }

protected:
    const VertexArray &getVertexArray() const override { return defaultVertexArray; }
    const ShaderProgram &getShaderProgram() const override { return particleSystemDrawShaderProgram; }
    const Texture2d &getTexture0() const override { return texture; }

private:
    class ParticleGroup
    {
    public:
        void generate(const glm::vec3 &center, float globalTime)
        {
            clock.reset();

            axes.setMatrixM(glm::translate(center));

            std::vector<float> position(verticesCount * 3);
            std::vector<float> velocity(verticesCount * 3);
            std::vector<float> other(verticesCount * 3); // <acceleration, lifetime, for future use>
            for (int i = 0; i < verticesCount; i++)
            {
                glm::vec3 currentPosition = center + glm::ballRand(0.01f);
                glm::vec3 currentVelocity = glm::ballRand(10.f);

                position[i * 3] = currentPosition.x;
                position[i * 3 + 1] = currentPosition.y;
                position[i * 3 + 2] = currentPosition.z;

                velocity[i * 3] = currentVelocity.x;
                velocity[i * 3 + 1] = currentVelocity.y;
                velocity[i * 3 + 2] = currentVelocity.z;

                other[i * 3] = glm::gaussRand(0.2f, 0.1f);   // acceleration
                other[i * 3 + 1] = glm::gaussRand(3.f, 1.f); // lifetime
                other[i * 3 + 2] = 1.f;
            }

            vertexArray.load(position, velocity, other);

            static int i = 0;
            ShaderProgram::setParticleGroupLight(ParticleGroupLight(center, globalTime), i);
            i++;
            i %= ParticleGroupLight::count;
        }
        const VertexArray &getVertexArray() const { return vertexArray; }
        float getTime() const { return clock.getTime(); }
        void drawAxes(Window *window) const { axes.draw(window); }

    private:
        static constexpr int verticesCount = 20000;
        VertexArray vertexArray;
        Clock clock;
        Axes axes;
    };
    static inline float globalTime;
    static inline ShaderProgram particleSystemDrawShaderProgram;
    static inline Texture2d texture;
    static constexpr int particleGroupCount = ParticleGroupLight::count;
    std::array<ParticleGroup, particleGroupCount> particleGroups;
};

#endif /* PARTICLESYSTEM_H */
