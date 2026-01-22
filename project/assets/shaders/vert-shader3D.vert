#version 460 core

in vec3 aPos;
in vec3 normal;
in vec2 texCoords;

uniform mat4 mvp;
uniform mat3 normalMat;
uniform mat4 model;

out vec3 fragPos;
out vec2 TexCoords;
out vec3 fragNormal;

void main()
{
    TexCoords = texCoords;
    fragNormal = normalMat * normal;
    gl_Position = mvp * vec4(aPos.xyz, 1.0);
    fragPos = (model * vec4(aPos.xyz, 1.0)).xyz;
}
