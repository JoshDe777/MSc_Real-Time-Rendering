#version 460 core

#define MAX_LIGHTS 1
#define PI 3.1415

struct PointLight{
    vec3 pos;
    vec3 emission;
    float I;
};

// Vertex Shader Input
in vec3 fragPos;
in vec3 fragNormal;
in vec2 TexCoords;

// Material x Texture Input
uniform sampler2D image;
uniform sampler2D backDepthMap;
uniform sampler2D frontDepthMap;
uniform float tiling;
uniform vec3 diffuse;
uniform float alpha;
uniform float metallic;
uniform float roughness;

// Lighting-related Input
uniform PointLight[MAX_LIGHTS] lights;
uniform int nLights;
uniform float ambient;
uniform float specular;
uniform vec3 camPos;
uniform int LOD;

uniform int screenWidth;
uniform int screenHeight;

out vec4 fragColor;

vec3 calculateFragColor(vec4 base){
    vec2 screenUV = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
    float zb = texture(backDepthMap, screenUV).r;
    float zf = texture(frontDepthMap, screenUV).r;

    float brightness = abs(zf - zb);
    if(brightness < 0.001)
        brightness = 0.001;

    vec3 result = specular * brightness * base.xyz;
    return result;
}

void main()
{
    if(LOD == 0) {
        vec4 base_color = ambient * vec4(diffuse.xyz, alpha) * texture(image, TexCoords * tiling);
        fragColor = vec4(base_color.xyz, alpha) ;
        return;
    }

    fragColor = vec4(calculateFragColor(vec4(diffuse, 1.0)).xyz, alpha) * texture(image, TexCoords * tiling);
}
