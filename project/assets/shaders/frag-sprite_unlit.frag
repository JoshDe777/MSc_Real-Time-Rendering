#version 460 core

in vec2 TexCoords;

uniform sampler2D image;
uniform vec4 diffuse;

out vec4 fragColor;

void main()
{
    fragColor = diffuse * texture(image, TexCoords);
}
