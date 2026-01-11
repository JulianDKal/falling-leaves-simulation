#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
// layout(location = 2) in mat4 modelMatrix;
layout(std430, binding = 0) buffer TransformBuffer {
    mat4 transforms[];
};

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aTexCoord;
    mat4 modelMatrix = transforms[gl_InstanceID];
    gl_Position = projection * view * modelMatrix * vec4(aPos, 1.0);
}