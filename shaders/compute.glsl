#version 450

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer ModelMatrixBuffer {
    mat4 modelMatrices[];
};
layout(std430, binding = 1) buffer InitialPositionsBuffer {
    vec4 initialPositions[];
};
layout(std430, binding = 2) buffer rotationsBuffer {
    vec4 rotations[];
};

layout(location = 0) uniform float emitHeight;
layout(location = 1) uniform float scale;

mat3 eulerToMat3(vec3 euler);

void main() {
    uint leafID = gl_GlobalInvocationID.x;
    if (leafID >= modelMatrices.length()) {
        return;
    }

    float fixedDT = 0.016;
    float rotationSpeed = 4.0;

    // Get current matrix
    mat4 model = modelMatrices[leafID];
    mat4 newModel = mat4(1.0);
    vec3 position = vec3(model[3]);

    // Update position
    position.y -= 0.1;
    if(position.y <= 0.0){
        position = vec3(initialPositions[leafID].x, emitHeight, initialPositions[leafID].z);
    }

    mat3 rotationMat = eulerToMat3(vec3(rotations[leafID].x, 0, rotations[leafID].z));
    float hello = scale;

    newModel[0] = vec4(rotationMat[0] * scale, 0.0);
    newModel[1] = vec4(rotationMat[1] * scale, 0.0);
    newModel[2] = vec4(rotationMat[2] * scale, 0.0);
    newModel[3] = vec4(position, 1.0);
    
    rotations[leafID] = vec4(rotations[leafID].x + fixedDT * rotationSpeed, 0, rotations[leafID].z + fixedDT * rotationSpeed, 0.0);
    modelMatrices[leafID] = newModel;
}

mat3 eulerToMat3(vec3 euler) {
    float cx = cos(euler.x);  // cos(pitch)
    float sx = sin(euler.x);  // sin(pitch)
    // float cy = cos(euler.y);  // cos(yaw)
    // float sy = sin(euler.y);  // sin(yaw)
    float cz = cos(euler.z);  // cos(roll)
    float sz = sin(euler.z);  // sin(roll)
    
    // Rotation matrix (XYZ order - common in games)
    mat3 rotX = mat3(1.0, 0.0, 0.0,
                     0.0, cx, -sx,
                     0.0, sx, cx);
                     
    // mat3 rotY = mat3(cy, 0.0, sy,
    //                  0.0, 1.0, 0.0,
    //                  -sy, 0.0, cy);
                     
    mat3 rotZ = mat3(cz, -sz, 0.0,
                     sz, cz, 0.0,
                     0.0, 0.0, 1.0);
    
    // Combine: Z * Y * X (typical order)
    return rotZ * rotX;
}