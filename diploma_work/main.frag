#version 330 core

in vec3 Pos;
in vec3 Normal;
in vec2 TPos;

uniform sampler2D text;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(1.f);
}