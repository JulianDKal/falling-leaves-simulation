#version 450 core

in vec2 TexCoord;
out vec4 fragmentColor;

uniform sampler2D leafTexture;

void main()
{
    // Sample the texture using UVs
    fragmentColor = texture(leafTexture, TexCoord);
}
