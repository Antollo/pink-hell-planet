#version 330

#define POSITION 0
#define COLOR 1
#define NORMAL 2
#define TEXCOORD 3

layout (location = POSITION) in vec3 inPosition;
layout (location = COLOR) in vec3 inColor;
layout (location = NORMAL) in vec3 inNormal;
layout (location = TEXCOORD) in vec2 inTexCoord;

layout (std140) uniform matrices
{
    mat4 P;
    mat4 V;
};

uniform mat4 M;

out vec4 outNormal;
out vec3 outColor;
out vec4 outPosition;

void main()
{
    outColor = inColor;
    outNormal = vec4(inNormal, 0);
    outPosition = vec4(inPosition, 1);
    gl_Position =  P * V * M * outPosition;
}