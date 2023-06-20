#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TPos;

struct PointLight {
    vec3 pos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 atten;
};

struct DirectionLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct FlashLight {
    vec3 pos;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    vec3 atten;
};

uniform PointLight pLight;
uniform DirectionLight dirLight;
uniform FlashLight flashLight;

struct ObjMaterial {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
};
uniform ObjMaterial material;

uniform sampler2D text;
uniform sampler2D caustic1;
uniform sampler2D caustic2;

uniform vec3 viewPos;
uniform float time;
uniform float fcSpeed;
uniform float scSpeed;

out vec4 outColor;

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);

    // Direction light
    vec3 lightDir = -normalize(dirLight.direction);
    vec3 lightReflDir = reflect(lightDir, Normal);

    float NdotL = max(dot(Normal, lightDir), 0);
    float RdotV = max(dot(lightReflDir, viewDir), 0);

    vec3 spec = pow(RdotV, material.shininess) * dirLight.specular * material.specular;
    vec3 diff = NdotL * material.diffuse * dirLight.diffuse;

    vec3 lc2 = spec + diff;
    // -------------------


    vec3 res = diff;
    res = dirLight.ambient * material.ambient + material.emission;
    //vec3 texRGB = vec3(texture(text, TPos));
    //vec2 cUV1 = (TPos + fcSpeed * time);
    //vec2 cUV2 = (TPos + scSpeed * time);
    //vec3 cRGB1 = vec3(texture(caustic1, cUV1));
    //vec3 cRGB2 = vec3(texture(caustic1, cUV2));
    //vec3 causticRGB = min(cRGB1, cRGB2);
    //res = mix(texRGB, causticRGB, 0.5);
    //res = vec3(1.f);
    outColor = vec4(min(res, 1.0f), 1.0f);
}