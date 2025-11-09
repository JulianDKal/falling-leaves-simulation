#version 450 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 texCoord;

out vec2 uv;
out vec4 vPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    uv = texCoord;
    vPos = projection * view * model * vec4(vertexPos, 1.0f);
    gl_Position = vPos;
}