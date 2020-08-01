#define POSITION 0
#define COLOR 1
#define NORMAL 2
#define TEXCOORD 3
#define TANGENT 4
#define BITANGENT 5

layout (location = POSITION) in vec3 position;
layout (location = COLOR) in vec3 color;
layout (location = NORMAL) in vec3 normal;
layout (location = TEXCOORD) in vec2 texCoord;
layout (location = TANGENT) in vec3 tangent;
layout (location = BITANGENT) in vec3 bitangent;
