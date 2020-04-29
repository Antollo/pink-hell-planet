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

uniform vec4 l = vec4(1, 1, 1, 0);

const vec4 eye = vec4(0, 0, 0, 1);

out vec3 outColor;

void main()
{
    gl_Position =  P * V * M * vec4(inPosition, 1);

    mat4 V_inv = mat4(inverse(transpose(mat3(V))));
    
    vec4 n = normalize(M*vec4(normal, 0));
    vec4 h = normalize(l + V_inv * eye);

    float nl = clamp(dot(n, l), 0, 1);

    float vl = clamp(dot(h, n), 0, 1);
    
    outColor = inColor * nl * 0.5 + inColor * pow(vl, 2) * 0.6;
}