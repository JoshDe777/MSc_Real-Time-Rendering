#version 460 core

in vec3 aPos;

uniform mat4 vp;

out vec3 TexCoords;

void main(){
    vec4 pos = vp * vec4(aPos.xyz, 1.0);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
    TexCoords = aPos.xyz;
}