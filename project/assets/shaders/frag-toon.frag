#version 460 core

#define MAX_LIGHTS 3

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
uniform int n_levels;

out vec4 fragColor;

vec3 calculateFragColor(vec4 base){
    float shiny = metallic * roughness;
    vec3 result = vec3(0, 0, 0); // ambient * base.xyz;
    vec3 normal = normalize(fragNormal);
    // apply diffuse and specular changes for each light affecting the object.
    for(int i = 0; i < nLights; i++){
        PointLight light = lights[i];
        // diffuse:
        vec3 lightDir = normalize(light.pos - fragPos);
        float diff_brightness = 1 - floor(max(0, dot(normal, -lightDir)) * n_levels) / n_levels;

        result += light.emission * base.xyz * diff_brightness;

        // specular:
        /*vec3 view = normalize(camPos - fragPos);
        float shinyFactor = min(1.0, shiny);
        float angle = max(0, dot(normal, normalize(reflect(lightDir, normal))));
        if (angle > 0.001)
            result += light.emission * pow(angle, shinyFactor * specular) * attenuation;*/
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
