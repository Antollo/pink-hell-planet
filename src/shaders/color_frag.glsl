#version 330

in float nl;
in float rv;
in vec3 outColor;

out vec4 color;

void main()
{
    color = vec4(outColor * nl, 1) + vec4(1, 0.9, 0.8, 1) * rv * 0.5;
}