#version 460 core

#define MAX_LIGHTS 1
#define PI 3.1415

struct PointLight{
    vec3 pos;
    vec3 emission;
    float I;
};

// VS inputs
in vec3 fragPos;
in vec2 TexCoords;
in vec3 fragNormal;
in vec3 fragTan;
in vec3 fragBitan;

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
uniform sampler2D nMap;

// Lighting-related Input
uniform PointLight[MAX_LIGHTS] lights;
uniform int nLights;
uniform int LOD;

// output(s)
out vec4 fragColor;

vec3 getNormalInWorldSpace(){
    // bump val to [-1, 1] for negative displacement (craters)
    vec3 mapNormal = texture(nMap, TexCoords).xyz;
    mapNormal = normalize(2 * (mapNormal - vec3(0.5)));

    vec3 normal = normalize(fragNormal);
    vec3 tan = normalize(fragTan);
    vec3 bitan = normalize(fragBitan);

    mat3 tanSpaceMat = mat3(tan, bitan, normal);

    return normalize(tanSpaceMat * mapNormal);
}

vec3 calculateFragColor(vec4 base){
    vec3 result = ambient * base.xyz;
    // calculate normal accounting for nMap
    vec3 normal = getNormalInWorldSpace();
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
