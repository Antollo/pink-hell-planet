#version 330

uniform sampler2D tex0;

in vec2 gTexCoord;
in float gAlpha;

out vec4 color;

void main()
{
    vec4 c = texture(tex0, gTexCoord);
    if (c.a == 0)
        discard;
    color = vec4(c.rgb, pow(c.a * gAlpha, 0.7));
}
