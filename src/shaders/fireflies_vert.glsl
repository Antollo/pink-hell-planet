#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

out vec4 vPositionVM;

void main()
{
    vPositionVM = V * M * vec4(position, 1);
	gl_Position = P * vPositionVM;
}