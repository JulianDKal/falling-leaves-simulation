#version 450

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer ModelMatrixBuffer {
    mat4 modelMatrices[];
};

layout(std430, binding = 1) buffer InitialPositionsBuffer {
    vec3 initialPositions[];
};

layout(location = 0) uniform float emitHeight;

void main() {
    uint leafID = gl_GlobalInvocationID.x;
    if (leafID >= modelMatrices.length()) {
        return;
    }

    // Get current matrix
    mat4 model = modelMatrices[leafID];
    vec3 position = vec3(model[3]);

    vec3 initialPos = initialPositions[leafID];

    // Update position
    position.y -= 0.1;
    if(position.y <= 0.0){
         position.y = emitHeight;
         position.x = initialPos.x;
    }
    
    // Create new matrix
    // mat4 newModel = model;  // Copy existing rotation/scale
    model[3] = vec4(position, 1.0);
    
    // Write back to SSBO (assign to array, not to 'model')
    modelMatrices[leafID] = model;
}