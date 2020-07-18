#ifndef DEBUG_H_
#define DEBUG_H_

#include <iostream>
#include <iomanip>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <LinearMath/btVector3.h>

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

inline std::ostream& operator<<(std::ostream& os, btVector3 vec)
{
    return os << "{ " << vec.x() << ", " << vec.y() << ", " << vec.z() << "}";
}

inline std::ostream& operator<<(std::ostream& os, glm::vec3 vec)
{
    os << std::setprecision(10);
    os << "{";
    for(int i = 0; i < 2; i++)
        os << vec[i] << ", ";
    os << vec[2] << "}";
    return os;
}

#define GL_ERROR_CHECK  {   GLenum err; \
                            while ((err = glGetError()) != GL_NO_ERROR) \
                                std::cerr << "OpenGL error " << std::hex << err \
                                << std::dec << " in " << __FILE__ <<  ":" << __LINE__ << std::endl; \
                        }

#endif