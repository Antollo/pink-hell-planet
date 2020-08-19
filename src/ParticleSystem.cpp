#include "ParticleSystem.h"

void ParticleSystem::init(GlobalConfig::GraphicSetting setting)
{
    particleSystemDrawShaderProgram.load("shaders/particleSystem_vert.glsl",
                                            "shaders/particleSystem_geom.glsl",
                                            "shaders/particleSystem_frag.glsl");

    if (setting == GlobalConfig::GraphicSetting::low)
        ParticleGroup::setVerticesCount(2000);

    texture.load("firefly_DIFF", true, true);

    for (int i = 0; i < particleGroupCount; i++)
        ShaderProgram::setParticleGroupLight(ParticleGroupLight(glm::vec3(0.f, 0.f, 0.f), -10.f), i);
}
