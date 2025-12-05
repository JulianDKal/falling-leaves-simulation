#include "Emitter.h"

int Emitter::instancesCount()
{
    return numInstances;
}

void Emitter::update()
{
    for (int i = 0; i < leaves.size(); i++)
        {
            leaves[i].addRotation(glm::vec3 {0, rotationSpeed, rotationSpeed});
            // leaves[i].draw(leafShader, view, projection);
        }
}

void Emitter::draw(const glm::mat4 &view, const glm::mat4 &projection)
{
}

Emitter::Emitter(int count)
{
    numInstances = count;
    leafShader.createProgram("./../shaders/triangle_vertex.glsl","./../shaders/triangle_fragment.glsl");
    leaves.reserve(numInstances);

    glGenVertexArrays(1, &leafVAO);
    glGenBuffers(1, &leafVBO);
    glBindVertexArray(leafVAO);
    glBindBuffer(GL_ARRAY_BUFFER, leafVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(leafVertices), leafVertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-5.0f, 5.0f);    // Position range
    std::uniform_real_distribution<float> rotDist(0.0f, 360.0f); 

    int idTracker = 0;

    

    for (int i = 0; i < 1000; i++)
    {
        glm::vec3 position {
            posDist(gen),  // x: -10 to 10
            posDist(gen),  // y: -10 to 10  
            posDist(gen)   // z: -10 to 0
        };
        
        glm::vec3 rotation {
            rotDist(gen),  // x rotation: -180 to 180 degrees
            rotDist(gen),  // y rotation: -180 to 180 degrees
            rotDist(gen)   // z rotation: -180 to 180 degrees
        };

        // glm::vec3 rotation {0.0f};
        
        Leaf l(position);
        l.setRotation(rotation);
        l.id = idTracker;
        idTracker++;
        leaves.push_back(l);
    }
}

Emitter::~Emitter()
{
}
