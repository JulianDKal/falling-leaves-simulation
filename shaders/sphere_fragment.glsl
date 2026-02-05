#version 450 core

in vec3 normal;
out vec4 fragmentColor;

void main()
{
    // vec3 lightPos = vec3(5, 5, 5);
    // vec3 lightVector = lightPos - vec3(vertPos) / length(lightPos - vec3(vertPos));

    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    float diffuse = max(dot(normal, lightDir), 0.0);

    vec3 color = vec3(0.7, 0.7, 0.7) * diffuse + vec3(0.2, 0.2, 0.2);

    fragmentColor = vec4(color, 1.0);
}
