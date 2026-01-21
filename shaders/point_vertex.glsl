#version 450 core

layout(location = 0) in vec3 aPos;
layout(std430, binding = 0) buffer TransformBuffer {
    mat4 transforms[];
};

uniform mat4 view;
uniform mat4 projection;
uniform float size;

void main()
{
    mat4 modelMatrix = transforms[gl_InstanceID];
    gl_Position = projection * view * modelMatrix * vec4(aPos, 1.0);
    gl_PointSize = size * 3.0;  // Set point size
}