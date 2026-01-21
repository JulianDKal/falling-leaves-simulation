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

void Emitter::draw(const glm::mat4 &view, const glm::mat4 &projection, const EmitterParams& params)
{
    getErrorCode();
    if(params.particleShape == ParticleShape::leafShape){
        glUseProgram(leafShader.ID);
        leafShader.useTexture(leafTexture, "leafTexture");

        getErrorCode();
        leafShader.setMatrix4("view", view);
        leafShader.setMatrix4("projection", projection);

        glBindVertexArray(leafVAO);

        int indexCount = sizeof(leafIndices) / sizeof(leafIndices[0]);
        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, numInstances);
    }
    else if(params.particleShape == ParticleShape::sphereShape){
        glUseProgram(sphereShader.ID);
        getErrorCode();
        sphereShader.setMatrix4("view", view);
        sphereShader.setMatrix4("projection", projection);

        glBindVertexArray(sphereVAO);

        glDrawElementsInstanced(GL_TRIANGLES, sphereIndices->size(), GL_UNSIGNED_INT, 0, numInstances);
        getErrorCode();
    }
    else if(params.particleShape == ParticleShape::pointShape) {
        glEnable(GL_PROGRAM_POINT_SIZE);
        glUseProgram(pointShader.ID);
        getErrorCode();
        pointShader.setMatrix4("view", view);
        pointShader.setMatrix4("projection", projection);
        pointShader.setFloat("size", params.size);

        glBindVertexArray(pointVAO);

        glDrawArraysInstanced(GL_POINTS, 0, 1, numInstances);
        getErrorCode();
        glDisable(GL_PROGRAM_POINT_SIZE);

    }
    
    getErrorCode();

}


void Emitter::resizeParticleCount(const EmitterParams &params)
{
    if(numInstances == params.leafCount) return; //Nothing to do
    //the incoming particle count is less than the current, so we just need to resize the buffers and not create any new leaves
    else if(numInstances > params.leafCount) {
        numInstances = params.leafCount;
        leaves.resize(numInstances);
        //TODO: Do we even need to resize the SSBOs?
        uploadInitialTransforms();
    }
    //The incoming particle count is bigger than the current, so we need to create new leaves
    else if(numInstances < params.leafCount){
        std::cout << "numInstances: " << numInstances << " " << " leafCount: " << params.leafCount << std::endl;

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_real_distribution<float> posDist(-params.emitRadius, params.emitRadius);
        std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
        std::uniform_real_distribution<float> oneDist(0.0f, 1.0f);
        std::uniform_real_distribution<float> spawnHeightDist(0.0f, params.emitHeight);

        leaves.reserve(params.leafCount);
        Profiler::Start();
        for (int i = numInstances; i < params.leafCount; i++)
        {
            leaves.emplace_back(createLeaf(params, gen, posDist, rotDist, oneDist, spawnHeightDist));
        }
        Profiler::Stop(1);
        numInstances = params.leafCount;
        uploadInitialTransforms();
    }
    
    std::cout << "Emitter buffers resized to size " << numInstances << std::endl;
}

void Emitter::changeEmitArea(const EmitterParams &params)
{
    //TODO: Do we really need to recreate every single leaf?
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> posDist(-params.emitRadius, params.emitRadius);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> oneDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> spawnHeightDist(0.0f, params.emitHeight);
    
    for (int i = 0; i < numInstances; i++)
    {
        leaves[i] = std::move(createLeaf(params, gen, posDist, rotDist, oneDist, spawnHeightDist));
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
    sphereShader.createProgram("./../shaders/sphere_vertex.glsl","./../shaders/sphere_fragment.glsl");
    pointShader.createProgram("./../shaders/point_vertex.glsl","./../shaders/point_fragment.glsl");
    computeShader.createComputeProgram("./../shaders/compute.glsl");
    leafTexture.initialize("./../textures/leaf-texture1.png", 0);

    leaves.reserve(numInstances);
    leaves.resize(numInstances);
    int sectorCount = 20, stackCount = 12;

    sphereCoordinates = generateSpherePoints(sectorCount, stackCount, 0.25f);
    sphereIndices = generateSphereIndices(sectorCount, stackCount);

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

    //Buffers for the sphere shape

    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);
    glBindVertexArray(sphereVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereCoordinates->size() * 3 * sizeof(float), sphereCoordinates->data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices->size() * sizeof(unsigned int), sphereIndices->data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Buffers for the point shape

    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), pointVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
    Leaf l{position, generateRandomRotation(gen, rotDist)};

    return l;
}

Emitter::~Emitter()
{
}


