#version 460 core

#define MAX_LIGHTS 3
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
    vec3 result = ambient * base.xyz;
    vec3 normal = normalize(fragNormal);
    // apply diffuse and specular changes for each light affecting the object.
    for(int i = 0; i < nLights; i++){
        PointLight light = lights[i];

    // diffuse:
        vec3 lightDir = normalize(light.pos - fragPos);
        float brightness = max(0, dot(normal, lightDir));
        // calculate distance between light src and obj.
        float dist = max(distance(light.pos, fragPos), 0.01);
        float attenuation = light.I / pow(dist, 3);
        result += light.emission * base.xyz * brightness * attenuation;

    // specular:
        vec3 view = normalize(camPos - fragPos);
        vec3 vHalf = normalize(lightDir + view);

        // fresnel reflectance approximation
        float m = max(0, dot(lightDir, vHalf));
        float F = pow((1 - m), 5) * (1 - specular) + specular;

        // beckmann facet vis - surface scattering approximation
        float angle = max(0, dot(normal, vHalf));
        float r1 = 1 / (4 * roughness * roughness * pow(angle, 4));
        float r2 = 0;
        if (angle > 0.001)
            // need alt value if roughness = 0
            r2 = (angle * angle - 1) / (roughness * roughness * angle * angle);
        float D = r1 * exp(r2);

        // Geometric shadowing (specular attenuation)
        float g = max(0, dot(normal, view));
        float g1 = (2.0 * angle * g) / m;
        float g2 = (2 * angle * brightness) / m;
        float G = min(1, min(g1, g2));

        float denumerator = PI * brightness * g;
        if(denumerator == 0)
            denumerator = 0.001;
        float ks = (F * D * G) / denumerator;

        result += brightness * light.emission * ks * attenuation;
    }
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
