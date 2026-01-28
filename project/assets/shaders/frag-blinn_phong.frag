#version 460 core

#define MAX_LIGHTS 1

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

out vec4 fragColor;

vec3 calculateFragColor(vec4 base){
    float shiny = 1.0 - roughness;
    vec3 result = ambient * base.xyz;
    vec3 normal = normalize(fragNormal);
    // apply diffuse and specular changes for each light affecting the object.
    for(int i = 0; i < nLights; i++){
        PointLight light = lights[i];
        // diffuse:
        vec3 lightDir = normalize(light.pos - fragPos);
        // calculate distance between light src and obj.
        float dist = max(distance(light.pos, fragPos), 0.01);
        float attenuation = light.I / (dist*dist);
        result += light.emission * base.xyz * max(0, dot(normal, lightDir)) * attenuation;

        // specular:
        float NdotL = dot(normal, lightDir);
        // only render if light in front of fragment
        if(NdotL > 0.0){
            vec3 view = normalize(camPos - fragPos);
            vec3 vHalf = normalize(lightDir + view);
            float shinyFactor = mix(1.0, specular, shiny);
            float angle = max(0.001, dot(normal, vHalf));
            result += light.emission * pow(angle, shinyFactor) * attenuation;
        }
    }
    return result;
}

void main()
{
    if(LOD == 0) {
        vec4 base_color = ambient * vec4(diffuse.xyz, alpha) * texture(image, TexCoords * tiling);
        fragColor = vec4(base_color.xyz, alpha);
        return;
    }

    fragColor = vec4(calculateFragColor(vec4(diffuse, 1.0)).xyz, alpha) * texture(image, TexCoords * tiling);
}
