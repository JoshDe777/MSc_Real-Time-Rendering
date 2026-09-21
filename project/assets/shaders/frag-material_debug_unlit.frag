#version 460 core

in vec2 TexCoords;

uniform sampler2D image;
uniform vec3 diffuse;
uniform float alpha;

out vec4 fragColor;

void main() {
    fragColor = vec4(diffuse , 1.0) * texture(image, TexCoords);
}
