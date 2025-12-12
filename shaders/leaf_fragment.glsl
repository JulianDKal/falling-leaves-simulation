#version 450 core

in vec2 TexCoord;
out vec4 fragmentColor;

uniform sampler2D leafTexture;

void main()
{
    // Sample the texture using UVs
    vec4 color = texture(leafTexture, TexCoord);
    if(color.a < 0.5) discard;
    fragmentColor = color;
}
