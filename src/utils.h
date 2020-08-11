#ifndef UTILS_H_
#define UTILS_H_

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include <array>

inline const btVector3& toBtVec3(const glm::vec3& vec)
{
    return *reinterpret_cast<const btVector3*>(&vec);
}

template<typename T>
inline constexpr std::array<T, 2> arrayFromPair(const std::pair<T, T>& pair)
{
    std::array<T, 2> arr = {pair.first, pair.second};
    return arr;
}

inline constexpr glm::vec3 remove4th(const glm::vec4& vec)
{
    return glm::vec3(vec.x, vec.y, vec.z);
}

#endif
