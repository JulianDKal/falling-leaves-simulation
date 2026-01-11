#include "Emitter.h"

int Emitter::instancesCount()
{
    return numInstances;
}

void Emitter::update(float dT, const EmitterParams& params)
{
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
    if(numInstances == params.leafCount) return; //Nothing to do
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);    // Position range
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f); 
    std::uniform_real_distribution<float> speedDist(0.5f, 4.0f);

    
    leaves.resize(params.leafCount);
    for (int i = numInstances; i < params.leafCount; i++)
    {
        glm::vec3 position {
            posDist(gen) ,  // x: -10 to 10
            (posDist(gen) + 10.0f) * 0.6,  // y: -0 to 12
            posDist(gen)   // z: -10 to 10
        };
        glm::vec3 rotation {
            rotDist(gen),  // x rotation: -180 to 180 degrees
            rotDist(gen),  // y rotation: -180 to 180 degrees
            rotDist(gen)   // z rotation: -180 to 180 degrees
        };

        Leaf l{position, speedDist(gen)};
        l.setRotation(rotation);
        leaves[i] = std::move(l);
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
    std::uniform_real_distribution<float> posDist;
    if(params.shape == EmitterShape::boxShape){
        posDist = std::uniform_real_distribution<float>(-params.emitRadius, params.emitRadius);    // Position range
    }
    else if(params.shape == EmitterShape::circleShape){
        posDist = std::uniform_real_distribution<float>(-params.emitRadius, params.emitRadius);    // Position range
    }
    std::uniform_real_distribution<float> speedDist(0.5f, 4.0f);
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f); 

    for (int i = 0; i < numInstances; i++)
    {
        glm::vec3 position {
            posDist(gen) ,
            params.emitHeight,
            posDist(gen)
        };
        
        glm::vec3 rotation {
            rotDist(gen),  // x rotation: -180 to 180 degrees
            rotDist(gen),  // y rotation: -180 to 180 degrees
            rotDist(gen)   // z rotation: -180 to 180 degrees
        };

        Leaf l{position, speedDist(gen)};
        l.setRotation(rotation);
        // leaves.emplace_back(Leaf{position, speedDist(gen)});
        leaves[i] = std::move(l);
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

Emitter::~Emitter()
{
}


