#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

out vec2 gTexCoords;

void main()
{
    gTexCoords = texCoord;
    gl_Position =  P * M * vec4(position.xyz, 1);
}
