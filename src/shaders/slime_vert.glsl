#version 330
#include "shaders/layouts.glsl"
#include "shaders/uniforms.glsl"

out vec4 gNormalVM;
out vec4 gPositionVM;
out vec2 gTexCoord;
out vec3 gNormal;
out vec3 gTangent;
out vec3 gBitangent;
out float gDistance;

out vec3 gViewerTBN;

vec4 slimeyFloat(vec4 v, float d)
{
    float m = clamp(d - 2, 0, 1) * clamp((30 - d) / 20, 0, 1) * 0.05;
    return vec4(m * (sin(time + v.x + 0.5 * v.y + 0.5 * v.z) - 0.5), m * (sin(time + v.y + 0.5 * v.x + 0.5 * v.z) - 0.5), m * (sin(time + v.z + 0.5 * v.x  + 0.5 * v.y) - 0.5), 0);
}

void main()
{
    gPositionVM = V * M * vec4(position, 1);
    gDistance = length(gPositionVM);
    gPositionVM += V * slimeyFloat(M * vec4(position, 1), gDistance);
    gNormalVM = V * M * vec4(normal, 0);
	gl_Position = P * gPositionVM;
    gTexCoord = texCoord;

    gNormal = normal;
    gTangent = tangent;
    gBitangent = bitangent;

    mat4 invTBN = transpose(mat4(vec4(gTangent, 0), vec4(gNormal, 0), vec4(gBitangent, 0), vec4(0, 0, 0, 1)));
    gViewerTBN = (invTBN * normalize(invV * vec4(0, 0, 0, 1) - vec4(position, 1))).xyz;
}