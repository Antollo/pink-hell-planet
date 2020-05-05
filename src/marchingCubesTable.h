#ifndef MARCHINGCUBESTABLE_H_
#define MARCHINGCUBESTABLE_H_

#include <vector>
#include <array>
#include <glm/glm.hpp>
#include "VecInt3.h"

extern const VecInt3 cubeVer[8];
extern const std::vector<std::vector<glm::vec3>> marchingCubesVertices;
extern const std::vector<std::vector<glm::vec3>> marchingCubesNormals;

#endif