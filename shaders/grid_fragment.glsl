#version 450 core

in vec2 uv;
in vec4 vPos;
out vec4 fragColor;
uniform sampler2D gridTexture;

void main() {
    vec3 normalizedPos = vPos.xyz / 50.0;
    float avg = abs((normalizedPos.x + normalizedPos.y + normalizedPos.z) / 3.0f);
    float alpha = 1.0f - avg;
    alpha = clamp(alpha, 0.0, 1.0);
    vec4 texColor = texture(gridTexture, uv);
    fragColor = vec4(texColor.rgb, alpha * 0.7);
}