#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>
#include "DrawableObject.h"

template<class T>
inline std::ostream& operator<<(std::ostream& os, std::vector<T> vec)
{
    os << "{";
    if(vec.size() > 0)
        for(size_t i = 0; ; i++)
        {
            os << vec[i];
            if (i == vec.size() - 1)
                break;
            os << ", ";
        }
    os << "}";
    return os;
}


inline std::ostream& operator<<(std::ostream& os, glm::vec3 vec)
{
    os << "{";
    for(int i = 0; i < 2; i++)
        os << vec[i] << ", ";
    os << vec[2] << "}";
    return os;
}


#endif