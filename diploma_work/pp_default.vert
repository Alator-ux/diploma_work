#version 410

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 texCoords;
out vec2 TexCoords;

void main() {
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0.0, 1.0f);
    TexCoords = texCoords;
}