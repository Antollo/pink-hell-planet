layout (std140) uniform data
{
    mat4 P;
    mat4 V;
    mat4 invV;
    float time;
};

uniform mat4 M;