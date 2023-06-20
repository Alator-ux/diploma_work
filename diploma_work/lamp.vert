#version 330 core

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

layout (location = 0) in vec3 vPos;

void main() {
    gl_Position = Projection * View * Model * vec4(vPos, 1.0f);
}