#version 460 core

#define MAX_LIGHTS 5

struct PointLight{
    vec3 pos;
    float _padding;     // required because of memory reading method according to Claude... (vec3 expected to start 16bit sequence so vec3 usage could read into the next struct)
    vec3 emission;
    float I;
};

// VS inputs
in vec3 fragPos;
in vec2 TexCoords;
in vec3 fragNormal;

// vec3s
uniform vec3 diffuse;
uniform vec3 camPos;

// floats
uniform float tiling;       // -------------- MATERIAL DATA --------------
uniform float alpha;
uniform float metallic;
uniform float roughness;
uniform float ambient;      // -------------- WORLD DATA --------------
uniform float specular;

// sampler2Ds
uniform sampler2D image;

// Lighting-related Input
layout(std430, binding=0) readonly buffer LightBuffer{
    PointLight[] lights;
};
uniform int nLights;

// Output(s)
out vec4 fragColor;

vec3 calculateFragColor(vec4 base){
    float shiny = 1.0 - roughness;
    vec3 result = ambient * base.xyz;

    // calculate normal accounting for nMap
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
    // removed LOD since can be done implicitly with nLights == 0 + branching is damaging for performance.
    fragColor = vec4(calculateFragColor(vec4(diffuse, 1.0)).xyz, alpha) * texture(image, TexCoords * tiling);
}
