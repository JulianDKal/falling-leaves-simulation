#version 450

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout(std430, binding = 0) buffer ModelMatrixBuffer {
    mat4 modelMatrices[];
};
layout(std430, binding = 1) buffer InitialPositionsBuffer {
    vec4 initialPositions[];
};
layout(std430, binding = 2) buffer rotationsBuffer {
    vec4 rotations[];
};
layout(std430, binding = 3) buffer velocityBuffer {
    vec4 velocities[];
};

layout(location = 0) uniform float emitHeight;
layout(location = 1) uniform float scale;
layout(location = 2) uniform float gravity;
layout(location = 3) uniform vec3 windForce;

mat3 eulerToMat3(vec3 euler);
vec3 applyForce(vec3 force, float mass);

void main() {
    //Compute a global ID for the current invocation from the workgroup size, workgroup ID and local Invocation IDs
    uint leafID = gl_WorkGroupID.x * gl_WorkGroupSize.x * gl_WorkGroupSize.y
                    + gl_LocalInvocationID.y * 16 + gl_LocalInvocationID.x;
                    
    if (leafID >= modelMatrices.length()) {
        return;
    }

    float fixedDT = 0.016;
    float rotationSpeed = 2.0;
    float mass = 1.0;
    float drag = 0.9;
    vec3 acceleration = vec3(0);
    vec3 velocity = vec3(velocities[leafID]);
    vec3 gravityForce = vec3(0.0, -gravity, 0.0);

    // Get current matrix
    mat4 model = modelMatrices[leafID];
    mat4 newModel = mat4(1.0);
    vec3 position = vec3(model[3]);

    // Update position
    acceleration += gravityForce / mass;
    acceleration += windForce * 5 / mass;
    velocity += acceleration * fixedDT * drag;
    position += velocity * fixedDT;

    if(position.y <= 0.0){
        position = vec3(initialPositions[leafID].x, emitHeight, initialPositions[leafID].z);
        velocities[leafID] = vec4(0);
    }
    else {
        velocities[leafID] = vec4(velocity, 0.0);
    }

    //rotate
    //TODO: Implement this with quaternions
    mat3 rotationMat = eulerToMat3(vec3(rotations[leafID].x, 0, rotations[leafID].z));

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

    mat3 rotX = mat3(1.0, 0.0, 0.0,
                     0.0, cx, -sx,
                     0.0, sx, cx);

    // mat3 rotY = mat3(cy, 0.0, sy,
    //                  0.0, 1.0, 0.0,
    //                  -sy, 0.0, cy);

    mat3 rotZ = mat3(cz, -sz, 0.0,
                     sz, cz, 0.0,
                     0.0, 0.0, 1.0);

    return rotZ * rotX;
}