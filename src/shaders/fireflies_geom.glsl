#version 330
#include "shaders/uniforms.glsl"
#include "shaders/fireflies.glsl"

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = firefliesVerticesCount) out;

in vec3 vPosition[];

out vec2 gTexCoord;
out float gAlpha;

void emitFirefly(vec4 posV)
{
    gAlpha = sin(time + posV.x + posV.y + posV.z) * 0.3 + 0.7;

    gl_Position = P * (posV + vec4(-0.5, -0.5, 0, 0));
    gTexCoord = vec2(0, 0);
    EmitVertex();
    gl_Position = P * (posV + vec4(0.5, -0.5, 0, 0));
    gTexCoord = vec2(1, 0);
    EmitVertex();
    gl_Position = P * (posV + vec4(-0.5, 0.5, 0, 0));
    gTexCoord = vec2(0, 1);
    EmitVertex();
    gl_Position = P * (posV + vec4(0.5, 0.5, 0, 0));
    gTexCoord = vec2(1, 1);
    EmitVertex();
    EndPrimitive();
}

void main(void)
{
    for(int i = 0; i < firefliesCount; i++)
        emitFirefly(V * fireflyPos(fireflies[i]));
}
