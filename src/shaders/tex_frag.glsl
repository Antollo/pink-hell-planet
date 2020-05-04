#version 330

uniform sampler2D tex;

in float nl;
in float rv;
in vec2 outTexCoord;

out vec4 color;

void main()
{
    float x = outTexCoord.x;
    float y = outTexCoord.y;
    color = vec4(texture(tex, vec2(x, 1-y)).rgb * nl, 1) + vec4(1, 0.9, 0.8, 1) * rv * 0.5;
}