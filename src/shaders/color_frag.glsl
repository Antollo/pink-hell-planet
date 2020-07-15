#version 330

layout (std140) uniform matrices
{
    mat4 P;
    mat4 V;
};

uniform mat4 M;

in vec4 outNormal;
in vec4 outPosition;
in vec3 outColor;

out vec4 color;

float rand3D(in vec3 co){
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,144.7272))) * 43758.5453);
}

float simple_interpolate(in float a, in float b, in float x)
{
   return a + smoothstep(0.0,1.0,x) * (b-a);
}
float interpolatedNoise3D(in float x, in float y, in float z)
{
    float integer_x = x - fract(x);
    float fractional_x = x - integer_x;

    float integer_y = y - fract(y);
    float fractional_y = y - integer_y;

    float integer_z = z - fract(z);
    float fractional_z = z - integer_z;

    float v1 = rand3D(vec3(integer_x, integer_y, integer_z));
    float v2 = rand3D(vec3(integer_x+1.0, integer_y, integer_z));
    float v3 = rand3D(vec3(integer_x, integer_y+1.0, integer_z));
    float v4 = rand3D(vec3(integer_x+1.0, integer_y +1.0, integer_z));

    float v5 = rand3D(vec3(integer_x, integer_y, integer_z+1.0));
    float v6 = rand3D(vec3(integer_x+1.0, integer_y, integer_z+1.0));
    float v7 = rand3D(vec3(integer_x, integer_y+1.0, integer_z+1.0));
    float v8 = rand3D(vec3(integer_x+1.0, integer_y +1.0, integer_z+1.0));

    float i1 = simple_interpolate(v1,v5, fractional_z);
    float i2 = simple_interpolate(v2,v6, fractional_z);
    float i3 = simple_interpolate(v3,v7, fractional_z);
    float i4 = simple_interpolate(v4,v8, fractional_z);

    float ii1 = simple_interpolate(i1,i2,fractional_x);
    float ii2 = simple_interpolate(i3,i4,fractional_x);

    return simple_interpolate(ii1 , ii2 , fractional_y);
}

float Noise3D(in vec3 coord, in float wavelength)
{
   return interpolatedNoise3D(coord.x/wavelength, coord.y/wavelength, coord.z/wavelength);
}


vec4 addLight(vec4 lp) 
{
    vec4 outNormal2 = outNormal + 0.1 * vec4(Noise3D((M * outPosition).yzx, 0.7), Noise3D((M * outPosition).xyz, 0.7), Noise3D((M * outPosition).zxy, 0.7), 0);

    vec4 l = normalize(V * lp - V * M * outPosition);
    vec4 n = normalize(V * M * outNormal2);
    vec4 r = reflect(-l, n);
    vec4 v = normalize(vec4(0, 0, 0, 1) - V * M * outPosition);

    float nl = clamp(dot(n, l), 0.1, 1);
    float rv = pow(clamp(dot(r, v), 0, 1), 6);
    float dist = distance(V * lp, V * M * outPosition);
    float att = 1 / (0.01 * pow(dist, 2) + 0.01 * dist + 1);

    return att * (vec4(outColor * nl * 0.5, 1) + vec4(1, 0.9, 0.8, 1) * rv * 0.5);
}

void main()
{
    color = addLight(vec4(10, 10, 10, 1));
}