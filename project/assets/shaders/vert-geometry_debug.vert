#version 460 core

in vec3 aPos;

uniform mat4 mv;
uniform mat4 mvp;

out float viewDepth;

void main()
{
    gl_Position = mvp * vec4(aPos.xyz, 1.0);
    vec4 viewPos = mv * vec4(aPos.xyz, 1.0);
    viewDepth = -viewPos.z;
}