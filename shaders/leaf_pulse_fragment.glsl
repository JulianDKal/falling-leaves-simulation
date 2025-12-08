#version 450 core

in vec2 TexCoord;
out vec4 fragmentColor;

uniform sampler2D leafTexture;
uniform float uTime;   // <- add this!

void main()
{
    vec4 base = texture(leafTexture, TexCoord);

    // Pulsation factor (smooth sinus)
    float pulse = 0.2 * sin(uTime * 4.0) + 1.0;
    // Range becomes: 0.8 – 1.2

    fragmentColor = vec4(base.rgb * pulse, base.a);
}