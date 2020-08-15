#version 330
#include "shaders/uniforms.glsl"
#include "shaders/fireflies.glsl"

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform samplerCube cube;

in vec4 gNormalVM;
in vec4 gPositionVM;
in vec2 gTexCoord;
in vec3 gNormal;
in vec3 gTangent;
in vec3 gBitangent;
in float gDistance;

in vec3 gViewerTBN;

out vec4 color;

vec4 addLight(vec4 lpV, vec4 v, vec4 n, vec3 diffuseTexColor, vec3 specularTexColor) 
{
    vec4 l = normalize(lpV - gPositionVM);
    vec4 r = reflect(-l, n);

    float nl = clamp(dot(n, l), 0.1, 1);
    float rv = pow(clamp(dot(r, v), 0, 1), 6);
    float dist = distance(lpV, gPositionVM);
    float att = (1 / (0.01 * pow(dist, 2) + 0.01 * dist + 1)) * 0.5;

    return vec4(att * (diffuseTexColor * nl + specularTexColor * rv), 0);
}

vec2 parallaxMapping(vec2 texCoords)
{
    const float h = 0.1;
    const float s = 12;
    vec2 tDelta = h * -gViewerTBN.xz / s;
    float hDelta = -1 / s;
    vec2 tc = texCoords;
    float hc = 1;
    float ht = texture(tex3, tc).r;
    if (gViewerTBN.y <= 0)
        discard;
    while (hc > ht)
    {
        tc += tDelta;
        hc += hDelta;
        ht = texture(tex3, tc).r;
    }

    vec2 tcOld = tc - tDelta;
    float hcOld = hc - hDelta;
    float htOld = texture(tex3, tcOld).r;

    float x = (hcOld - htOld) / (hcOld - htOld - (hc - ht));

    return mix(tcOld, tc, x);
}

void main()
{
    vec2 texCoords = gTexCoord;
    // Parallax mapping = 30-40 fps on integrated graphics
    if  (gDistance < 12)
    {
        float m = clamp((-gDistance + 12) / 3, 0, 1);
        texCoords = mix(gTexCoord, parallaxMapping(gTexCoord), m);
    }
    
    vec4 v = normalize(vec4(0, 0, 0, 1) - gPositionVM);
    vec4 n = gNormalVM;

    if (gDistance < 50)
    {
        float m = clamp((-gDistance + 50) / 20, 0, 1);
        mat4 TBN = mat4(vec4(gTangent, 0), vec4(gNormal, 0), vec4(gBitangent, 0), vec4(0, 0, 0, 1));
        n = normalize(V * M * TBN * vec4(-1 + 2 * texture(tex2, texCoords).rbg, 0) * m + gNormalVM * (1 - m));
    }
    
    color = vec4(0.005 * texture(tex0, texCoords).rgb, 1);

    vec3 diffuseTexColor = texture(tex0, texCoords).rgb;
    vec3 specularTexColor = texture(tex1, texCoords).rgb;

    for (int i = 0; i < firefliesCount; i++)
        color += addLight(V * fireflyPos(fireflies[i]), v, n, diffuseTexColor, specularTexColor);

    for (int i = 0; i < particleGroupLightCount; i++)
    {
        if (-lights[i].startTime + time > 6)
            continue;
        float m = clamp(pow(5 / (-lights[i].startTime + time + 0.1) - 0.5, 3), 0, 2);
        color += addLight(V * vec4(lights[i].position, 1), v, n, mix(diffuseTexColor, vec3(1, 0.5, 0.5), 0.1), mix(specularTexColor, vec3(1, 0.5, 0.5), 0.7)) * m;
    }

    vec3 r = mat3(invV) * reflect(-v.xyz, n.xyz);
    color += sqrt(1 - pow(dot(v.xyz, n.xyz), 2)) * 0.1 * vec4(texture(cube, r).rgb, 0);

}
