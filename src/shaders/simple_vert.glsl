#version 330

#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

out vec3 outColor;

void main()
{
    outColor = color;
    gl_Position =  P * V * M * vec4(position, 1);
}
