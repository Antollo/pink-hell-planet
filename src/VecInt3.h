#ifndef VECINT_H_
#define VECINT_H_

#include <iostream>
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using VecInt3 = std::array<int, 3>;

inline VecInt3 getVecInt3(int x, int y, int z)
{
    return {x, y, z};
}

inline VecInt3 operator+(VecInt3 a, VecInt3 b)
{
    for (int i = 0; i < 3; i++)
        a[i] += b[i];
    return a;
}

inline VecInt3 operator-(VecInt3 a, VecInt3 b)
{
    for (int i = 0; i < 3; i++)
        a[i] -= b[i];
    return a;
}

inline VecInt3 operator*(VecInt3 a, int b)
{
    for (auto& i : a)
        i *= b;
    return a;
}

inline glm::vec3 VecInt3ToVec3(VecInt3 v)
{
    return glm::vec3(v[0], v[1], v[2]);
}

inline btVector3 getBtVector3(VecInt3 v)
{
    return btVector3(v[0], v[1], v[2]);
}

inline std::ostream& operator<<(std::ostream& os, VecInt3 vec)
{
    os << "{";
    for(int i = 0; i < 2; i++)
        os << vec[i] << ", ";
    os << vec[2] << "}";
    return os;
}

#endif
