#version 460 core

in vec2 TexCoords;

uniform sampler2D image;
uniform float tiling;
uniform vec3 diffuse;

out vec4 fragColor;

void main()
{
    fragColor = vec4(diffuse.xyz, 1.0) * texture(image, TexCoords * tiling);
}
