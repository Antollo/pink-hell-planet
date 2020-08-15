#version 330
#include "shaders/uniforms.glsl"

uniform sampler2D tex0;

in vec2 gTexCoords;

out vec4 color;

void main()
{
    color = vec4(globalColor.rgb, alpha * texture(tex0, gTexCoords).r);
}
