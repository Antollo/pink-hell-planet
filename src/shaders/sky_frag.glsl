#version 330

uniform samplerCube cube;

in vec3 gTexCoord;

out vec4 color;

void main()
{    
    color = texture(cube, gTexCoord);
}