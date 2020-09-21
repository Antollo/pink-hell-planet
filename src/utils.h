#ifndef UTILS_H_
#define UTILS_H_

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <array>

constexpr float pi = 3.14159265358979323846;

inline const btVector3& toBtVec3(const glm::vec3& vec)
{
    return *reinterpret_cast<const btVector3*>(&vec);
}

inline const glm::vec3& toGlmVec3(const btVector3& vec)
{
    return *reinterpret_cast<const glm::vec3*>(&vec);
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

template<typename T>
inline constexpr bool isZeroOrNormal(T v)
{
    return v == 0 or std::isnormal(v);
}

inline constexpr bool isVec3Ok(glm::vec3 v)
{
    return isZeroOrNormal(v.x) and isZeroOrNormal(v.y) and isZeroOrNormal(v.z);
}

#endif
