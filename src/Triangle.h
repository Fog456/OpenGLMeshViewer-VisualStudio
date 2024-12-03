#pragma once

#include <glm/glm.hpp>

class Triangle
{
public:
    Triangle() {};
    glm::vec3* getPos()
    {
        return kPositions;
    }
    glm::vec4* getColor()
    {
        return kColors;
    }
    unsigned int getNum()
    {
        return num;
    }
private:
    glm::vec3 kPositions[3] = {
        glm::vec3{ -1.0f, -1.0f,0.0f},
        glm::vec3{  1.0f, -1.0f,0.0f},
        glm::vec3{   0.f,  1.0f,0.0f}
    };

    glm::vec4 kColors[3] = {
        glm::vec4{ 1.f, 0.f, 0.f ,1.f},
        glm::vec4{ 0.f, 1.f, 0.f ,1.f},
        glm::vec4{ 0.f, 0.f, 1.f ,1.f}
    };

    unsigned int num = 3;
};
