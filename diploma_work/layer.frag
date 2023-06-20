#version 410

in vec2 TexCoords;

const int di = 0;
const int sp = 1;
const int tr = 2;
const int em = 3;
const int gi = 4;
const int ca = 5;

uniform sampler2D layers[6];
uniform int active[6];
uniform float g_mult;
uniform float ca_mult;
uniform float brightness;
uniform float exposure;
uniform float gamma;
uniform bool HDR_ON;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outBloom;

vec3 HDR(vec3 rgb) {
    // vec3 mapped = vec3(1.0) - exp(-rgb * exposure);
    // mapped = pow(mapped, vec3(1.0 / gamma));
    // return mapped;
    float t = pow(exposure, -1);
    vec3 mapped = vec3(1.0) - exp(-t * rgb);
    mapped = pow(mapped, vec3(1.0 / gamma));
    return mapped;
}

void main()
{
    vec2 invertedCoords = vec2(TexCoords.s, 1.0 - TexCoords.t);
    vec3 rgb = vec3(0.0);
    rgb += active[di] * texture2D(layers[di], invertedCoords).rgb;
    rgb += active[sp] * texture2D(layers[sp], invertedCoords).rgb;
    rgb += active[tr] * texture2D(layers[tr], invertedCoords).rgb;

    vec3 emRGB = active[em] * texture2D(layers[em], invertedCoords).rgb;
    rgb += emRGB;

    rgb += active[gi] * texture2D(layers[gi], invertedCoords).rgb;
    rgb += active[ca] * texture2D(layers[ca], invertedCoords).rgb;

    if (HDR_ON) {
        rgb = HDR(rgb);
    }
    else {
        rgb = clamp(rgb, 0.0, 1.0);
    }
    outBloom = vec4(0.0, 0.0, 0.0, 1.0);
    if(emRGB != vec3(0.0)) {
        outBloom = vec4(1.0);
    }
    outColor = vec4(rgb, 1.0);
}