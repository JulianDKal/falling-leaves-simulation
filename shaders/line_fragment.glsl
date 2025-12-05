#version 450 core

uniform vec3 color;
in vec4 vertexPos;
out vec4 fragmentColor;

void main() {
    float normalized = abs(length(vertexPos.xyz) / 150.f);
    float alpha = 1 - normalized - 0.5;
    fragmentColor = vec4(color, alpha);
}