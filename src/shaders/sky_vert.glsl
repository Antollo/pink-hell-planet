#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

out vec3 gTexCoord;

void main()
{
    gTexCoord = position;
    gl_Position = (P * vec4(mat3(V) * position, 1.0)).xyww;
}