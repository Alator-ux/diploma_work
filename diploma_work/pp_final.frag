#version 410

in vec2 TexCoords;

uniform sampler2D final;

out vec4 outColor;

void main()
{
    outColor = vec4(texture(final, TexCoords).rgb, 1.0);
}