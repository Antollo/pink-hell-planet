#version 330
#include "shaders/uniforms.glsl"

out vec4 color;

void main()
{
    color = vec4(globalColor.rgb, alpha);
}
