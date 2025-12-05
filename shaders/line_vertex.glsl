#version 450 core

layout (location = 0) in vec3 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 vertexPos;

void main() {
    vertexPos = projection * view * model * vec4(vPos, 1.0);
    gl_Position = vertexPos;
}