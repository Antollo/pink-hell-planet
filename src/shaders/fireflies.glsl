struct Firefly
{
    vec3 n;
    vec3 f;
    vec3 s;
    vec3 t;
};

const float timeScale = 0.1;
#define firefliesCount 4
#define firefliesVerticesCount 16

const Firefly fireflies[firefliesCount] = Firefly[firefliesCount](
    Firefly(
        vec3(3, 7, 2),
        vec3(0.7, 0, 0.2),
        vec3(10, 1, 10),
        vec3(10, 27, 10)
    ),
    Firefly(
        vec3(3, 5, 2),
        vec3(1.5, 0, 0.2),
        vec3(10, 1, 10),
        vec3(30, 27, 10)
    ),
    Firefly(
        vec3(3, 7, 5),
        vec3(0.7, 0, 1),
        vec3(10, 1, 10),
        vec3(10, 27, 30)
    ),
    Firefly(
        vec3(3, 7, 4),
        vec3(0.1, 0, 0.7),
        vec3(10, 1, 10),
        vec3(30, 27, 30)
    )
);

vec4 fireflyPos(Firefly a)
{
    float t = time * timeScale;
    // trajectories of Lissajous knots
    return vec4(a.s * vec3(cos(a.n.x * t + a.f.x), cos(a.n.y * t + a.f.y), cos(a.n.z * t + a.f.z)), 0) + vec4(a.t, 1);
}
