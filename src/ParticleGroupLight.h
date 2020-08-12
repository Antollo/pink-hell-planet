#ifndef PARTICLEGROUPLIGHT_H_
#define PARTICLEGROUPLIGHT_H_

#include <glm/glm.hpp>

class ParticleGroupLight
{
public:
    ParticleGroupLight(const glm::vec3 &position, float time)
        : groupPosition(position), startTime(time) {}
        
    static constexpr int count = 10;
private:
    glm::vec3 groupPosition;
    float startTime;
};

#endif /* PARTICLEGROUPLIGHT_H_ */
