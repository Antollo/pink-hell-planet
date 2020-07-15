#version 330

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

layout (std140) uniform matrices
{
    mat4 P;
    mat4 V;
};

uniform mat4 M;

in vec4 gNormalVM;
in vec4 gPositionVM;
in vec2 gTexCoord;
in vec3 gNormal;
in vec3 gTangent;
in vec3 gBitangent;
in float gDistance;
//in vec3 gColorDebug;

out vec4 color;


vec4 addLight(vec4 lpV, vec4 v, vec4 n, vec2 texCoords) 
{
    vec4 l = normalize(lpV - gPositionVM);
    vec4 r = reflect(-l, n);

    float nl = clamp(dot(n, l), 0.1, 1);
    float rv = pow(clamp(dot(r, v), 0, 1), 6);
    float dist = distance(lpV, gPositionVM);
    float att = (1 / (0.01 * pow(dist, 2) + 0.01 * dist + 1)) * 0.5;

    return vec4(texture(tex0, texCoords).rgb * nl * att + texture(tex1, texCoords).rgb * rv * att, 0);
}

void main()
{
    vec2 texCoords = vec2(gTexCoord.x, 1 - gTexCoord.y);
    vec4 v = normalize(vec4(0, 0, 0, 1) - gPositionVM);
    //vec4 n = normalize(V * M * TBN * normalize(vec4(-1 + 2 * texture(tex2, texCoords).rbg, 0)));

    vec4 n;
    float m = 1;


    if (gDistance > 24)
    {
        n = gNormalVM;
    }
    else
    {
        float m = clamp((-gDistance + 24) / 4, 0, 1);
        mat4 TBN = mat4(vec4(gTangent, 0), vec4(gNormal, 0), vec4(gBitangent, 0), vec4(0, 0, 0, 1));
        n = normalize(V * M * TBN * vec4(-1 + 2 * texture(tex2, texCoords).rbg, 0) * m + gNormalVM * (1 - m));
    }
    
    //color = vec4(gColorDebug * 0.2, 1);
    color = vec4(0.05 * m * texture(tex0, texCoords).rgb, 1);
    color += addLight(V * vec4(10, 10, 10, 1), v, n, texCoords);
    color += addLight(vec4(0, 1, -2, 1), v, n, texCoords);
}