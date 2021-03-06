#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

#define velocity color
#define acceleration vec3(0, -normal.x, 0)
#define lifetime normal.y

out vec4 vPositionVM;
out float vLifetime;

void main()
{
    vPositionVM = V * M * vec4(position + time * velocity  + time * time * (acceleration - velocity * 0.1), 1);
    vLifetime = lifetime;
}
