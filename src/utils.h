#ifndef UTILS_H_
#define UTILS_H_

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

inline const btVector3& toBtVec3(const glm::vec3& vec)
{
    return *reinterpret_cast<const btVector3*>(&vec);
}

#endif