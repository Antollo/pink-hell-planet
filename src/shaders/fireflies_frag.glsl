#version 330
#include "shaders/uniforms.glsl"

uniform sampler2D tex0;

in vec2 gTexCoord;

out vec4 color;

void main()
{
    color = texture(tex0, gTexCoord);
}