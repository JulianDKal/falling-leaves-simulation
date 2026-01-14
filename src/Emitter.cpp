#include "Emitter.h"


void Emitter::update(float dT, const EmitterParams& params)
{

    // --- Fixed timestep physics ---
    physicsAccumulator += dT;
    
    glUseProgram(computeShader.ID);
    computeShader.setFloat("emitHeight", params.emitHeight);
    computeShader.setFloat("scale", params.size);
    computeShader.setFloat("gravity", params.gravity);
    computeShader.setVec3f("windForce", params.windForce);

    while (physicsAccumulator >= fixedDT)
    {
        fixedUpdatePhysics(fixedDT);
        physicsAccumulator -= fixedDT;
    }
}

void Emitter::fixedUpdatePhysics(float fixedDT)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transformationsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positionsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, rotationsSSBO);
    // Dispatch compute shader
    glDispatchCompute(numInstances, 1, 1);
    // Wait for compute to finish
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void Emitter::draw(const glm::mat4 &view, const glm::mat4 &projection)
{
    getErrorCode();
    glUseProgram(leafShader.ID);
    leafShader.useTexture(leafTexture, "leafTexture");

    getErrorCode();
    leafShader.setMatrix4("view", view);
    leafShader.setMatrix4("projection", projection);

    glBindVertexArray(leafVAO);

    int indexCount = sizeof(leafIndices) / sizeof(leafIndices[0]);
    glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, numInstances);

    getErrorCode();

}


void Emitter::resizeParticleCount(const EmitterParams &params)
{
    if(numInstances == params.leafCount) return;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> posDist(-params.emitRadius, params.emitRadius);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> speedDist(0.5f, 4.0f);
    std::uniform_real_distribution<float> oneDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> spawnHeightDist(1.0f, params.emitHeight);

    leaves.resize(params.leafCount);
    for (int i = numInstances; i < params.leafCount; i++)
    {
        leaves[i] = std::move(createLeaf(params, gen, posDist, rotDist, speedDist, oneDist, spawnHeightDist));
    }
        
    uploadInitialTransforms();
    std::cout << "Emitter buffers resized!" << std::endl;
}

void Emitter::changeEmitArea(const EmitterParams &params)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> posDist(-params.emitRadius, params.emitRadius);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> speedDist(0.5f, 4.0f);
    std::uniform_real_distribution<float> oneDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> spawnHeightDist(1.0f, params.emitHeight);

    for (int i = 0; i < numInstances; i++)
    {
        leaves[i] = std::move(createLeaf(params, gen, posDist, rotDist, speedDist, oneDist, spawnHeightDist));
    }
    uploadInitialTransforms();

    std::cout << "Emit Area changed!" << std::endl;
}

void Emitter::uploadInitialTransforms() {
        std::vector<glm::mat4> initialTransforms(numInstances);
        //Might be able to remove this later if the leaves can respawn randomly in the compute shader
        std::vector<glm::vec4> initialPositions(numInstances); 
        std::vector<glm::vec4> rotations(numInstances);
        for (int i = 0; i < numInstances; i++) {
            initialTransforms[i] = leaves[i].getLeafModel();
            //Need to make these vec4 because of memory alignment reasons in the compute shader - might be able to fix later?
            initialPositions[i] = glm::vec4(leaves[i].getPosition(), 1.0f);
            rotations[i] = glm::vec4(leaves[i].getRotation(), 1.0f);
        }
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, transformationsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::mat4), initialTransforms.data(), GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::vec4), initialPositions.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, rotationsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::vec4), rotations.data(), GL_DYNAMIC_DRAW);

    }

Emitter::Emitter(const EmitterParams& params)
{
    numInstances = params.leafCount;

    leafShader.createProgram("./../shaders/leaf_vertex.glsl","./../shaders/leaf_fragment.glsl");
    computeShader.createComputeProgram("./../shaders/compute.glsl");
    leafTexture.initialize("./../textures/leaf-texture1.png", 0);

    leaves.reserve(numInstances);
    leaves.resize(numInstances);

    //Set up the Shader Storage Buffer Object for the leaf model matrices
    glGenBuffers(1, &transformationsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, transformationsSSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); 
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transformationsSSBO);

    glGenBuffers(1, &positionsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionsSSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positionsSSBO);

    glGenBuffers(1, &rotationsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, rotationsSSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, rotationsSSBO);


    //Generate buffers for the leaf object that will be used for instancing
    glGenVertexArrays(1, &leafVAO);
    glGenBuffers(1, &leafVBO);
    glGenBuffers(1, &leafEBO);
    
    glBindVertexArray(leafVAO);

    glBindBuffer(GL_ARRAY_BUFFER, leafVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leafVertices), leafVertices, GL_DYNAMIC_DRAW);
    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, leafEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(leafIndices), leafIndices, GL_STATIC_DRAW);
    // POS (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // UV  (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //This creates all the leaves and assigns random positions to them as well as fill the ssbo with matrices
    changeEmitArea(params);

}

glm::vec3 Emitter::generateRandomRotation(std::mt19937 &gen, std::uniform_real_distribution<float> &rotDist) {
    return glm::vec3{
        rotDist(gen),
        rotDist(gen),
        rotDist(gen)
    };
}

Leaf Emitter::createLeaf(const EmitterParams &params, std::mt19937 &gen,
                         std::uniform_real_distribution<float> &posDist,
                         std::uniform_real_distribution<float> &rotDist,
                         std::uniform_real_distribution<float> &speedDist,
                         std::uniform_real_distribution<float> &oneDist,
                         std::uniform_real_distribution<float> &spawnHeightDist)
{
    glm::vec3 position;

    // Random spawn height from pre-made distribution
    float spawnHeight = spawnHeightDist(gen);

    if(params.shape == EmitterShape::boxShape) {
        position = glm::vec3{
            posDist(gen),    // X
            spawnHeight,     // Y
            posDist(gen)     // Z
        };
    }
    else if(params.shape == EmitterShape::circleShape) {
        position = glm::vec3{1, 0, 0};
        glm::quat rotation = glm::angleAxis(glm::radians(rotDist(gen)), glm::vec3{0, 1, 0});

        // Correct uniform distribution over the circle
        float r = sqrt(oneDist(gen)) * params.emitRadius;
        position = rotation * position * r + glm::vec3{0, spawnHeight, 0};
    }

    glm::vec3 rotation = generateRandomRotation(gen, rotDist);
    Leaf l{position, speedDist(gen)};
    l.setRotation(rotation);

    return l;
}

Emitter::~Emitter()
{
}


