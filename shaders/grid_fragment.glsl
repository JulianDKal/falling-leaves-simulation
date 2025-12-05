#version 450 core

in vec2 uv;
in vec4 vPos;
out vec4 fragColor;
uniform sampler2D gridTexture;

void main() {
    vec2 normalizedPos = vec2(abs(vPos.x / 250), abs(vPos.z / 250));
    float avg = length(normalizedPos);
    float alpha = clamp(1.0f - avg - 0.7, 0.0, 1.0);
    vec4 texColor = texture(gridTexture, uv);
    fragColor = vec4(texColor.rgb, alpha);
}