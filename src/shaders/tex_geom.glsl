#version 330

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 6) out;

layout (std140) uniform matrices
{
    mat4 P;
    mat4 V;
};

uniform mat4 M;

in vec3 vNormal[];
in vec3 vPosition[];
in vec2 vTexCoord[];
in vec3 vTangent[];
in vec3 vBitangent[];

out vec4 gNormalVM;
out vec4 gPositionVM;
out vec2 gTexCoord;
out vec3 gNormal;
out vec3 gTangent;
out vec3 gBitangent;
out float gDistance;

out vec3 gColorDebug;

void main(void)
{
    /*vec3 e1 = vPosition[1].xyz - vPosition[0].xyz;
    vec3 e2 = vPosition[2].xyz - vPosition[0].xyz;

    vec2 t1 = vTexCoord[1] - vTexCoord[0];
	vec2 t2 = vTexCoord[2] - vTexCoord[0];

    float r = 1.0 / (t1.x * t2.y - t1.y * t2.x);

    vec4 tangent = vec4(normalize((e1 * t2.y - e2 * t1.y)*r), 0);*/

	for (int i = 0; i < gl_in.length(); i++)
    {
        gPositionVM = V * M * vec4(vPosition[i], 1);
        gDistance = length(gPositionVM);
        gNormalVM = V * M * vec4(vNormal[i], 0);
		gl_Position = P * gPositionVM;
        gTexCoord = vTexCoord[i];

        /*if (i == 0)
            gColorDebug = vec3(1, 0, 0);
        if (i == 1)
            gColorDebug = vec3(0, 1, 0);
        if (i == 2)
            gColorDebug = vec3(0, 0, 1);*/

        //gTBN = mat4(tangent, vec4(vNormal[i], 0), vec4(normalize(cross(tangent.xyz, vNormal[i].xyz)), 0), vec4(0, 0, 0, 1));
        //gTBN = mat4(vec4(vTangent[i], 0), vec4(vNormal[i], 0), vec4(vBitangent[i], 0), vec4(0, 0, 0, 1));

        gNormal = vNormal[i];
        gTangent = vTangent[i];
        gBitangent = vBitangent[i];

		EmitVertex();
	}
    EndPrimitive();
}
