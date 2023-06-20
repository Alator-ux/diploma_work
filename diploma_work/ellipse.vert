#version 330 core
    

in vec3 vPos;
in vec3 vColor;

uniform vec2 ab;
uniform mat4 el_scale;

out vec2 rDot;
out vec2 gDot;
out vec2 bDot;
out float mixValueCoeff;
out vec3 Color;
out vec2 Pos;
void main() {
    vec2 startPos = vec2(0.5, 0.5);
    float startR = 0.5;
    rDot = startPos;
    rDot.y += startR;
    gDot = startPos;
    gDot.y -= startR / 2;
    gDot.x += sqrt(2)/2*startR;
    bDot = startPos;
    bDot.y -= startR / 2;
    bDot.x -= sqrt(2)/2*startR;
    
    rDot.y *= ab.y;
    rDot.x *= ab.x;
    gDot.y *= ab.y;
    gDot.x *= ab.x;
    bDot.y *= ab.y;
    bDot.x *= ab.x;
    mixValueCoeff = 1 + 5*(2 - (ab.x + ab.y));
    float y = vPos.y * ab.y;
    float x = vPos.x * ab.x;
    gl_Position = vec4(x, y, 0.0, 1.0);
    Color = vColor;
    Pos = vec2(vPos);
}