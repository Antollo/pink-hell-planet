#ifndef B3632A49_012B_4F4C_BD5B_E61C2C9575A1
#define B3632A49_012B_4F4C_BD5B_E61C2C9575A1

#include <cmath>
#include <glm/glm.hpp>
#include "Clock.h"

class Wind
{
public:
    static float getStrength()
    {
        const float t = windClock.getTime();
        return 0.3f * std::sin(t + 1.3f) * std::cos(2.1f * t) * std::sin(3.5f * t);
    }
    static glm::vec3 getImpulse(float delta)
    {
        const float t = windClock.getTime();
        return getStrength() * delta * glm::normalize(glm::vec3(
            std::sin(t + 1.1f) * std::cos(2.3f * t),
            std::sin(t + 2.8f) * std::cos(2.1f * t),
            std::cos(t + 1.7f) * std::sin(2.2f * t)));
    }

private:
    static inline Clock windClock;
};

#endif /* B3632A49_012B_4F4C_BD5B_E61C2C9575A1 */
