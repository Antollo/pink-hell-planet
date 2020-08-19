#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

out vec4 gNormalVM;
out vec4 gNormalM;
out float dist;
out vec4 gPositionVM;
out vec3 gNormal;
out vec3 gTangent;
out vec3 gBitangent;

void main()
{
    gl_Position =  P * V * M * vec4(position, 1);
    gNormalVM = V * M * normalize(vec4(normal, 0));
    gNormalM = M * normalize(vec4(normal, 0));
    dist = gl_Position.w;
    gPositionVM = V * M * vec4(position, 1);
    gNormal = normal;
    gTangent = tangent;
    gBitangent = bitangent;
}
