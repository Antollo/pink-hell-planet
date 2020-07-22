#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

out vec4 gNormalVM;
out vec4 gPositionVM;
out vec2 gTexCoord;
out vec3 gNormal;
out vec3 gTangent;
out vec3 gBitangent;

out vec3 gPosition;

void main()
{
    gPosition = position;
    gPositionVM = V * M * vec4(position, 1);
    gNormalVM = V * M * vec4(normal, 0);
	gl_Position = P * gPositionVM;
    gTexCoord = texCoord;

    gNormal = normal;
    gTangent = tangent;
    gBitangent = bitangent;
}