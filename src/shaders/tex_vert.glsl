#version 330

#define POSITION 0
#define COLOR 1
#define NORMAL 2
#define TEXCOORD 3

layout (location = POSITION) in vec3 inPosition;
layout (location = COLOR) in vec3 inColor;
layout (location = NORMAL) in vec3 normal;
layout (location = TEXCOORD) in vec2 inTexCoord;

layout (std140) uniform matrices
{
    mat4 P;
    mat4 V;
};

uniform mat4 M;

const vec4 eye = vec4(0, 0, 0, 1);

out float nl;
out float rv;
out vec2 outTexCoord;

void main()
{
    outTexCoord = inTexCoord;
    gl_Position =  P * V * M * vec4(inPosition, 1);

    vec4 lp = vec4(1, 1, 1, 1);

    vec4 l = normalize(V * lp - V * M * vec4(inPosition, 1));
    vec4 n = normalize(V * M * vec4(normal, 0));
    vec4 r = reflect(-l, n);
    vec4 v = normalize(vec4(0, 0, 0, 1) - V * M * vec4(inPosition, 1));

    nl = clamp(dot(n, l), 0.1, 1);
    rv = pow(clamp(dot(r, v), 0, 1), 6);
}