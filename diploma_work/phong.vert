#version 330 core
layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TPos;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main() {
    vec4 newPos = Model * vec4(vPos, 1.0);
    gl_Position = Projection * View * newPos;
    FragPos = vec3(newPos);
    Normal = mat3(transpose(inverse(Model))) * vNormal;
	TPos = vec2(vTPos.x, vTPos.y);
}