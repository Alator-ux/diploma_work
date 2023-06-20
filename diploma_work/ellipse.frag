#version 330 core

in vec2 rDot;
in vec2 gDot;
in vec2 bDot;
in float mixValueCoeff;
in vec3 Color;
in vec2 Pos;
out vec4 color;

const float v = 1.0;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
    float sat = distance(Pos, vec2(0.0));
    vec2 hue_sc = Pos / sat;
    float hue = acos(hue_sc.x) * sign(hue_sc.y);
    hue = hue / (3.1415 * 2);// * 180.0;

    vec3 rgb = hsv2rgb(vec3(hue, sat, v));
   
    color = vec4(rgb,1.0);
}