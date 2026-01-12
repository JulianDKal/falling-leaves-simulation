#include "Emitter.h"


void Emitter::update(float dT, const EmitterParams& params)
{
    // --- Fixed timestep physics ---
    physicsAccumulator += dT;

    while (physicsAccumulator >= fixedDT)
    {
        fixedUpdatePhysics(fixedDT);
        physicsAccumulator -= fixedDT;
    }
    glUseProgram(computeShader.ID);
    computeShader.setFloat("emitHeight", params.emitHeight);
    // Bind SSBO to binding point 0
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transformationsSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positionsSSBO);
    // Dispatch compute shader
    glDispatchCompute(numInstances, 1, 1);
    // Wait for compute to finish
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Emitter::fixedUpdatePhysics(float fixedDT)
{
    for (auto& leaf : leaves)
    {
        //leaf.physicsUpdate(fixedDT); // physics-only update
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


void Emitter::resizeParticleCount(const EmitterParams &params)
{
    if(numInstances == params.leafCount) return; //Nothing to do
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-params.emitRadius, params.emitRadius); // Position range
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f); 
    std::uniform_real_distribution<float> speedDist(0.5f, 4.0f);
    std::uniform_real_distribution<float> oneDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> heightDist(1.0f, params.emitHeight);

    leaves.resize(params.leafCount);
    glm::vec3 position;
    for (int i = numInstances; i < params.leafCount; i++)
    {
        if(params.shape == EmitterShape::boxShape) {
            position = glm::vec3{
                posDist(gen) ,
                heightDist(gen),
                posDist(gen)
            };
        }
        else if(params.shape == EmitterShape::circleShape) {
            position = glm::vec3{1, 0, 0};
            glm::quat rotation = glm::angleAxis(glm::radians(rotDist(gen)), glm::vec3{0, 1, 0});
            position = rotation * position * oneDist(gen) * params.emitRadius + glm::vec3{0, heightDist(gen), 0};
        }
        glm::vec3 rotation {
            rotDist(gen),  
            rotDist(gen),  
            rotDist(gen)
        };

        Leaf l{position, speedDist(gen)};
        l.setRotation(rotation);
        leaves[i] = std::move(l);
    }
    
    // transformations.resize(params.leafCount, glm::mat4(1.0));
    numInstances = params.leafCount;
    uploadInitialTransforms();
    // glBindBuffer(GL_ARRAY_BUFFER, transformationsVBO);
    // glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), transformations.data(), GL_DYNAMIC_DRAW);

    std::cout << "Emitter buffers resized!" << std::endl;
}

void Emitter::changeEmitArea(const EmitterParams &params)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-params.emitRadius, params.emitRadius);
    std::uniform_real_distribution<float> speedDist(0.5f, 4.0f);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f);
    std::uniform_real_distribution<float> oneDist(0.0f, 1.0f);
    std::uniform_real_distribution<float> heightDist(1.0f, params.emitHeight);

    glm::vec3 position;
    for (int i = 0; i < numInstances; i++)
    {
        if(params.shape == EmitterShape::boxShape) {
            position = glm::vec3{
                posDist(gen) ,
                heightDist(gen),
                posDist(gen)
            };
        }
        else if(params.shape == EmitterShape::circleShape) {
            position = glm::vec3{1, 0, 0};
            glm::quat rotation = glm::angleAxis(glm::radians(rotDist(gen)), glm::vec3{0, 1, 0});
            position = rotation * position * oneDist(gen) * params.emitRadius + glm::vec3{0, heightDist(gen), 0};
        }
        
        glm::vec3 rotation {
            rotDist(gen),  
            rotDist(gen),
            rotDist(gen)
        };

        Leaf l{position, speedDist(gen)};
        l.setRotation(rotation);
        // leaves.emplace_back(Leaf{position, speedDist(gen)});
        leaves[i] = std::move(l);
    }
    uploadInitialTransforms();

    std::cout << "Emit Area changed!" << std::endl;
}

void Emitter::uploadInitialTransforms() {
        std::vector<glm::mat4> initialTransforms(numInstances);
        std::vector<glm::vec4> initialPositions(numInstances);
        for (int i = 0; i < numInstances; i++) {
            initialTransforms[i] = leaves[i].getLeafModel();
            initialPositions[i] = glm::vec4(leaves[i].getPosition(), 1.0f);
            // std::cout << "Position " << i << ": (" << initialPositions[i].x << ", " << initialPositions[i].y << ", " << initialPositions[i].z << ")" << std::endl;
        }
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, transformationsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::mat4), initialTransforms.data(), GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionsSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::vec4), initialPositions.data(), GL_DYNAMIC_DRAW);

    }

Emitter::Emitter(const EmitterParams& params)
{
    numInstances = params.leafCount;

    leafShader.createProgram("./../shaders/leaf_vertex.glsl","./../shaders/leaf_fragment.glsl");
    computeShader.createComputeProgram("./../shaders/compute.glsl");
    leafTexture.initialize("./../textures/leaf-texture1.png", 0);

    leaves.reserve(numInstances);
    leaves.resize(numInstances);
    transformations.reserve(numInstances);
    transformations.resize(numInstances, glm::mat4(1.0));

    //Set up the Shader Storage Buffer Object for the leaf model matrices
    glGenBuffers(1, &transformationsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, transformationsSSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); 
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, transformationsSSBO);

    glGenBuffers(1, &positionsSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, positionsSSBO);

    glBufferData(GL_SHADER_STORAGE_BUFFER, numInstances * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, positionsSSBO);

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
    // glGenBuffers(1, &transformationsVBO);
    // glBindBuffer(GL_ARRAY_BUFFER, transformationsVBO);
    
    // // Allocate buffer memory for all transformation matrices
    // glBufferData(GL_ARRAY_BUFFER, numInstances * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW); 
    
    //Have to do this 4 times because apparently we can't make a matrix take up one attribute slot
    // glBindBuffer(GL_ARRAY_BUFFER, transformationsSSBO);
    // for (int i = 0; i < 4; i++) {
    //     GLuint attribLocation = 2 + i;  // Start after pos and UV
    
    //     glEnableVertexAttribArray(attribLocation);
        
    //     glVertexAttribPointer(attribLocation, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        
    //     //The 1 means that this attribute advances 1 per instance, not per vertex
    //     //When putting in 0 we would move on to the next transformation every time the vertex shader runs (for every vertex)
    //     glVertexAttribDivisor(attribLocation, 1);
    // }

    //This creates all the leaves and assigns random positions to them as well as fill the ssbo with matrices
    changeEmitArea(params);

}

Emitter::~Emitter()
{
}


