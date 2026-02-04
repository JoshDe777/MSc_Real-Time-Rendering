#version 460 core

in float viewDepth;

out float outDepth;

void main(){
    outDepth = viewDepth;
}