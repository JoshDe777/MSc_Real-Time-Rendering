#version 460 core

in vec3 aPos;
in vec2 UV;

uniform mat4 mvp;

out vec2 TexCoords;

void main() {
    gl_Position = mvp * vec4(aPos.xyz, 1.0);
    TexCoords = UV;
}
