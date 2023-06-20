#version 410

in vec2 TexCoords;

uniform sampler2D mainTex;
uniform sampler2D bloomTex;
uniform float bloom_intensity = 1.0;
out vec4 outColor;

void main()
{
    vec3 mainRgb = texture2D(mainTex, TexCoords).rgb;
    vec3 bloomRgb = texture2D(bloomTex, TexCoords).rgb;
    outColor = vec4(min(mainRgb + bloomRgb * bloom_intensity, 1.0), 1.0);
}