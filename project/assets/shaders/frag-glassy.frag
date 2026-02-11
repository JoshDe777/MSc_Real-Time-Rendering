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

// Vec3s
uniform vec3 diffuse;
uniform vec3 eta;
uniform vec3 camPos;

// floats
uniform float tiling;
uniform float alpha;
uniform float metallic;
uniform float roughness;
uniform float ambient;
uniform float specular;

// integers
uniform int screenWidth;
uniform int screenHeight;

// Samplers (broken)
uniform sampler2D image;
uniform sampler2D nMap;
uniform samplerCube cubeMap;
uniform sampler2D backDepthMap;
uniform sampler2D frontDepthMap;

// output(s)
out vec4 fragColor;

vec3 getNormalInWorldSpace(){
    vec3 mapNormal = texture(nMap, TexCoords).xyz;
    mapNormal = normalize(2 * (mapNormal - vec3(0.5)));

    vec3 normal = normalize(fragNormal);
    vec3 tan = normalize(fragTan);
    vec3 bitan = normalize(fragBitan);

    mat3 tanSpaceMat = mat3(tan, bitan, normal);

    return normalize(tanSpaceMat * mapNormal);
}

vec3 calculateFragColor(vec4 base){
    // establish reflection dir [reflect(-view, normal)]
    // calculate normal accounting for nMap
    vec3 normal = getNormalInWorldSpace();
    vec3 view = normalize(camPos - fragPos);
    vec3 reflectionDir = reflect(-view, normal);
    vec3 reflection = texture(cubeMap, reflectionDir).xyz;

    // refraction - chromatic aberrations by scattering the refraction angles by color channel
    vec3 rRefractDir = refract(-view, normal, eta.r);
    // guardrails for total reflection
    if(length(rRefractDir) < 0.001)
        rRefractDir = reflectionDir;
    vec3 gRefractDir = refract(-view, normal, eta.g);
    if(length(gRefractDir) < 0.001)
        gRefractDir = reflectionDir;
    vec3 bRefractDir = refract(-view, normal, eta.b);
    if(length(bRefractDir) < 0.001)
        bRefractDir = reflectionDir;

    vec3 refraction = vec3(
        texture(cubeMap, rRefractDir).r,
        texture(cubeMap, gRefractDir).g,
        texture(cubeMap, bRefractDir).b
    );

    // m should be view.normal?
    float m = max(0, dot(view, normal));
    float F = pow((1 - m), 5) * (1 - specular) + specular;

    // reflection color calculation; blended using Fresnel approximation.
    vec3 cLight = mix(refraction, reflection, F);

    // depth component for transmittance effects
    vec2 screenUV = gl_FragCoord.xy / vec2(screenWidth, screenHeight);
    float zb = texture(backDepthMap, screenUV).r;
    float zf = texture(frontDepthMap, screenUV).r;

    float depth = abs(zf - zb);
    if(depth < 0.001)
        depth = 0.001;

    vec3 result = exp(-ambient * depth) * cLight * base.xyz;
    return result;
}

void main()
{
    // maybe add the alpha value somewhere to make the surface glassier?
    fragColor = vec4(calculateFragColor(vec4(diffuse, 1.0)).xyz, alpha);
}
