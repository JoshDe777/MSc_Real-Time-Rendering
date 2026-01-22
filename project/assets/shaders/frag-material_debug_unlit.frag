#version 460 core

uniform vec4 diffuse;

out vec4 fragColor;

void main() {
    fragColor = diffuse;
}
