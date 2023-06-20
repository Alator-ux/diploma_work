#version 410

in vec2 TexCoords;

uniform sampler2D tex;
uniform float grain_amount;   // Интенсивность эффекта grain
uniform float grain_size;     // Размер шумовых зерен

out vec4 outColor;

float random(vec2 p)
{
  return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
    
    vec3 rgb = texture2D(tex, TexCoords).rgb;
    rgb = vec3(dot(rgb, vec3(0.299, 0.587, 0.114)));
    vec2 rndSeed = TexCoords;
    vec3 grain = vec3(random(rndSeed) * grain_amount);
    rgb = mix(rgb, grain, grain_size);
    outColor = vec4(rgb, 1.0);
}
