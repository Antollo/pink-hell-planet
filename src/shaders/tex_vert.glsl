#version 330

#define POSITION 0
#define COLOR 1
#define NORMAL 2
#define TEXCOORD 3
#define TANGENT 4
#define BITANGENT 5

layout (location = POSITION) in vec3 position;
layout (location = NORMAL) in vec3 normal;
layout (location = TEXCOORD) in vec2 texCoord;
layout (location = TANGENT) in vec3 tangent;
layout (location = BITANGENT) in vec3 bitangent;

layout (std140) uniform matrices
{
    mat4 P;
    mat4 V;
};

uniform mat4 M;

out vec3 vNormal;
out vec3 vPosition;
out vec2 vTexCoord;
out vec3 vTangent;
out vec3 vBitangent;

void main()
{
    vTexCoord = texCoord;
    vNormal = normal;
    vPosition = position;
    vTangent = tangent;
    vBitangent = bitangent;
    //gl_Position =  P * V * M * vPosition;
}