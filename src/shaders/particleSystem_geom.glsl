#version 330
#include "shaders/uniforms.glsl"

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

in vec4 vPositionVM[];
in float vLifetime[];

out vec2 gTexCoord;
out float gAlpha;

void main()
{
    float size = 0.5;
    if (graphicSetting == graphicSettingLow)
        size = 1;
    if (vLifetime[0] >= time)
    {
        gAlpha = sqrt(clamp(vLifetime[0] - time, 0, 1));

        gl_Position = P * (vPositionVM[0] + vec4(size, -size, 0, 0));
        gTexCoord = vec2(0, 0);
        EmitVertex();
        gl_Position = P * (vPositionVM[0] + vec4(size, -size, 0, 0));
        gTexCoord = vec2(1, 0);
        EmitVertex();
        gl_Position = P * (vPositionVM[0] + vec4(-size, size, 0, 0));
        gTexCoord = vec2(0, 1);
        EmitVertex();
        gl_Position = P * (vPositionVM[0] + vec4(size, size, 0, 0));
        gTexCoord = vec2(1, 1);
        EmitVertex();
        EndPrimitive();
    }
}
