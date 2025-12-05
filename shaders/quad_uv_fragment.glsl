#version 450 core

in vec2 TexCoord;
out vec4 fragmentColor;

uniform float uTime;

// Sinusoidal interpolation function
float sinusInterp(float t)
{
    return 0.5 * (1.0 - cos(t * 3.1415926));
}

void main()
{
    float speed = 3.0; // higher = faster color change

    // Base color from UV
    float rBase = sinusInterp(TexCoord.x);
    float gBase = sinusInterp(TexCoord.y);
    float bBase = 0.5;

    // Faster sinus color transition
    float r = 0.5 + 0.5 * sin(uTime * speed + rBase * 3.1415926);
    float g = 0.5 + 0.5 * sin(uTime * speed + gBase * 3.1415926 + 1.0);
    float b = 0.5 + 0.5 * sin(uTime * speed + bBase * 3.1415926 + 2.0);

    fragmentColor = vec4(r, g, b, 1.0);
}
