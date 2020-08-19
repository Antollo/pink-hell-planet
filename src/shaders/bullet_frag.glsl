#version 330
#include "shaders/uniforms.glsl"
#include "shaders/fireflies.glsl"

out vec4 color;

uniform sampler2D tex0;

in vec4 gNormalVM;
in vec4 gNormalM;
in float dist;
in vec4 gPositionVM;
in vec3 gNormal;
in vec3 gTangent;
in vec3 gBitangent;

const float xTexRes = 50;
const float yTexRes = 50;
const float xDistMul = -6.9;
const float yDistMul = 11.2;

const vec4 dirDiffuseDir = normalize(vec4(0.4, -1, 0.7, 0));
const vec4 dirDiffuseColor = vec4(0.042, 0.015, 0, 0);

const vec4 specularColor = vec4(0.3, 0.3, 0.1, 0);

vec4 getSpecular(vec4 lightPosition, vec4 fNormalVM, vec4 viewDir)
{
    vec4 lightVM = V * lightPosition;
    vec4 reflectDir = reflect(normalize(gPositionVM - lightVM), fNormalVM);
    float specular = pow(clamp(dot(viewDir, reflectDir), 0.0, 1.0), 5.5);
    float dist = distance(lightVM, gPositionVM);
    float att = (1 / (0.01 * pow(dist, 2) + 0.01 * dist + 1));
    return specular * specularColor * att;
}

void main()
{
    color = vec4(0, 0, 0, 0);
    vec4 viewDir = normalize(vec4(0, 0, 0, 1) - gPositionVM);

    mat4 TBN = mat4(vec4(gTangent, 0), vec4(gNormal, 0), vec4(gBitangent, 0), vec4(0, 0, 0, 1));

    vec4 fNormalM = normalize(M * TBN * vec4(0, 1, 0, 0));
    vec4 fNormalVM = normalize(V * fNormalM);

    vec2 coords2d = vec2(mod(gl_FragCoord.x + dist * xDistMul, xTexRes) / xTexRes, mod(gl_FragCoord.y + dist * yDistMul, yTexRes) / yTexRes);
    color = texture(tex0, coords2d);

    float diffuse = max(max(dot(dirDiffuseDir, fNormalM), dot(-dirDiffuseDir, fNormalM)), 0);
    color += diffuse * dirDiffuseColor;

    vec4 v = normalize(vec4(0, 0, 0, 1) - gPositionVM);
    for (int i = 0; i < particleGroupLightCount; i++)
    {
        if (-lights[i].startTime + time > 6)
            continue;
        float specularTime = clamp(pow(5 / (-lights[i].startTime + time + 0.1) - 0.5, 3), 0, 2);
        color += getSpecular(vec4(lights[i].position, 1), fNormalVM, viewDir) * specularTime * 6;
    }

    for (int i = 3; i < 4; i++)
    {
        color += getSpecular(fireflyPos(fireflies[i]), fNormalVM, viewDir) * 2;
    }
}
