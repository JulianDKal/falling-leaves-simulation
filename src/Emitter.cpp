#include "Emitter.h"

int Emitter::instancesCount()
{
    return numInstances;
}

void Emitter::update(float dT, const EmitterParams& params)
{
    totalTime += dT;  // accumulate

    // --- Fixed timestep physics ---
    physicsAccumulator += dT;

    while (physicsAccumulator >= fixedDT)
    {
        fixedUpdatePhysics(fixedDT);
        physicsAccumulator -= fixedDT;
    }
    Profiler::Start();
    for (int i = 0; i < numInstances; i++)
    {
        leaves[i].addRotation(glm::vec3 {0, rotationSpeed, rotationSpeed});
        leaves[i].update(params);
        transformations[i] = leaves[i].getLeafModel();
    }
    updateTransformBuffer();
    Profiler::Stop(100);
}

void Emitter::fixedUpdatePhysics(float fixedDT)
{
    for (auto& leaf : leaves)
    {
        leaf.physicsUpdate(fixedDT); // physics-only update
    }
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

void Emitter::setTimeUniform(float time)
{ 
    glUseProgram(leafShader.ID);        // make shader active
    leafShader.setFloat("uTime", time); // set the uniform
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

    transformations.resize(params.leafCount, glm::mat4(1.0));
    numInstances = params.leafCount;

    glBindBuffer(GL_ARRAY_BUFFER, transformationsVBO);
    glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), transformations.data(), GL_DYNAMIC_DRAW);

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

    std::cout << "Emit Area changed!" << std::endl;
}

Emitter::Emitter(const EmitterParams& params)
{
    numInstances = params.leafCount;
    std::cout << numInstances << std::endl;

    leafShader.createProgram("./../shaders/leaf_vertex.glsl","./../shaders/leaf_fragment.glsl");
    leafTexture.initialize("./../textures/leaf-texture1.png", 0);

    leaves.reserve(numInstances);
    leaves.resize(numInstances);
    transformations.reserve(numInstances);
    transformations.resize(numInstances, glm::mat4(1.0));

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

    //Generate the VBO for the transformation matrices
    glGenBuffers(1, &transformationsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, transformationsVBO);
    
    // Allocate buffer memory for all transformation matrices
    glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); 
    
    //Have to do this 4 times because apparently we can't make a matrix take up one attribute slot
    for (int i = 0; i < 4; i++) {
        GLuint attribLocation = 2 + i;  // Start after pos and UV
    
        glEnableVertexAttribArray(attribLocation);
        
        glVertexAttribPointer(attribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        
        //The 1 means that this attribute advances 1 per instance, not per vertex
        //When putting in 0 we would move on to the next transformation every time the vertex shader runs (for every vertex)
        glVertexAttribDivisor(attribLocation, 1);
    }

    changeEmitArea(params);
}

void Emitter::updateTransformBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, transformationsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, transformations.size() * sizeof(glm::mat4), transformations.data());
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
        position = rotation * position * oneDist(gen) * params.emitRadius + glm::vec3{0, spawnHeight, 0};
    }

    glm::vec3 rotation = generateRandomRotation(gen, rotDist);
    Leaf l{position, speedDist(gen)};
    l.setRotation(rotation);

    return l;
}

Emitter::~Emitter()
{
}


