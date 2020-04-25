#version 330

#define POSITION 0
#define COLOR 1
#define NORMAL 2

layout (location = POSITION) in vec3 inPosition;
layout (location = COLOR) in vec3 inColor;
layout (location = NORMAL) in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform vec4 lightDir = vec4(0 ,0 ,1, 0);

out vec3 outColor;

void main()
{
    gl_Position =  P * V * M * vec4(inPosition, 1);
    mat4 G = mat4(inverse(transpose(mat3(M))));
    vec4 n = normalize(V * G * vec4(normal, 0));
    float nl = clamp(dot(n, lightDir), 0, 1);
    outColor=inColor * nl;
}