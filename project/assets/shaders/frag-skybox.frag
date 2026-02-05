#version 460 core

in vec3 TexCoords;

uniform samplerCube cubeMap;

out vec4 fragColor;

void main(){
    fragColor = vec4(texture(cubeMap, TexCoords).xyz, 1.0);
}
