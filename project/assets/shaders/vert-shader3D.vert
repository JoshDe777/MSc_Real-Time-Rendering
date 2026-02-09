#version 460 core

in vec3 aPos;
in vec3 normal;
in vec2 texCoords;
in vec3 tan;
in vec3 bitan;

uniform mat4 mvp;
uniform mat3 normalMat;
uniform mat4 model;

out vec3 fragPos;
out vec2 TexCoords;
out vec3 fragNormal;
out vec3 fragTan;
out vec3 fragBitan;

void main()
{
    TexCoords = texCoords;
    fragNormal = normalMat * normal;
    fragTan = normalMat * tan;
    fragBitan = normalMat * bitan;
    gl_Position = mvp * vec4(aPos.xyz, 1.0);
    fragPos = (model * vec4(aPos.xyz, 1.0)).xyz;
}
