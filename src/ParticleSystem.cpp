#include "ParticleSystem.h"

void ParticleSystem::init(GlobalConfig::GraphicSetting setting)
{
    particleSystemDrawShaderProgram.load("shaders/particleSystem_vert.glsl",
                                            "shaders/particleSystem_geom.glsl",
                                            "shaders/particleSystem_frag.glsl");

    if (setting == GlobalConfig::GraphicSetting::low)
        verticesCount = 2000; 

    texture.load("firefly_DIFF", true, true);

    for (int i = 0; i < particleGroupCount; i++)
        ShaderProgram::setParticleGroupLight(ParticleGroupLight(glm::vec3(0.f, 0.f, 0.f), -10.f), i);
    
    std::vector<float> position(verticesCount * 3);
    std::vector<float> velocity(verticesCount * 3);
    std::vector<float> other(verticesCount * 3); // <acceleration, lifetime, for future use>
    #pragma omp parallel for
    for (int i = 0; i < verticesCount; i++)
    {
        glm::vec3 currentPosition = glm::ballRand(0.01f);
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

    vertexArray.loadVerticesColorsNormals(position, velocity, other);
}
