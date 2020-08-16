#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

void main()
{
    gl_Position =  P * M * vec4(position, 1);
}
