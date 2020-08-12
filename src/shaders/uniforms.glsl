struct ParticleGroupLight
{
    vec3 position;
    float startTime;
};

#define particleGroupLightCount 10

layout (std140) uniform data
{
    mat4 P;
    mat4 V;
    mat4 invV;
    float time;
    ParticleGroupLight lights[particleGroupLightCount];
    // "The array stride (the bytes between array elements) is always rounded up to the size of a vec4"
    // https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout
    // I spent way too much time debugging it.
};

uniform mat4 M;
