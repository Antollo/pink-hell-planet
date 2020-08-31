#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>
#include <array>
#include "Clock.h"
#include "DrawableObject.h"
#include "GlobalConfig.hpp"
#include "Axes.h"

class ParticleSystem : public DrawableObject
{
public:
    static void init(GlobalConfig::GraphicSetting setting);

    void draw(Window *window) const override
    {
        getShaderProgram().use();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, getTexture0().getTextureId());
        getShaderProgram().setUniform1i("tex0", 0);
        getShaderProgram().validate();
        float temp = ShaderProgram::getTime();
        for (auto &particleGroup : particleGroups)
        {
            if (particleGroup.getTime() > 6.f)
                continue;
            ShaderProgram::setTime(particleGroup.getTime());
            getShaderProgram().setUniformMatrix4fv("M", particleGroup.getM());
            glBindVertexArray(vertexArray.getVertexArrayId());
            glDrawArrays(GL_POINTS, 0, vertexArray.getLength());
        }
        ShaderProgram::setTime(temp);
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
    const ShaderProgram &getShaderProgram() const override { return particleSystemDrawShaderProgram; }
    const Texture2d &getTexture0() const override { return texture; }

private:
    class ParticleGroup
    {
    public:
        void generate(const glm::vec3 &center, float globalTime)
        {
            clock.reset();

            M = glm::translate(center);
            axes.setMatrixM(M);

            static int i = 0;
            ShaderProgram::setParticleGroupLight(ParticleGroupLight(center, globalTime), i);
            i++;
            i %= ParticleGroupLight::count;
        }

        float getTime() const { return clock.getTime(); }
        glm::mat4 getM() const { return M; }

        void drawAxes(Window *window) const { axes.draw(window); }

    private:
        glm::mat4 M;
        
        Clock clock;
        Axes axes;
    }; 

    static inline int verticesCount = 20000;
    static inline VertexArray vertexArray;
    static inline float globalTime;
    static inline ShaderProgram particleSystemDrawShaderProgram;
    static inline Texture2d texture;
    static constexpr int particleGroupCount = ParticleGroupLight::count;

    std::array<ParticleGroup, particleGroupCount> particleGroups;
};

#endif /* PARTICLESYSTEM_H */
