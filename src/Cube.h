#pragma once

#include <glm/glm.hpp>

class Cube {
public:
    Cube() {};
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
    glm::vec3 kPositions[36] = {
        //Front
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),

        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),

        //back
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),

        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),

        //left
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),

        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),

        //right
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),

        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),

        //up
        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),

        glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f),

        //down
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),

        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
    };

    glm::vec4 kColors[36] = {
        //Front
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),

        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),

        //back
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),

        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),
        glm::vec4(1, 0, 0, 1),

        //left
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),

        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),

        //right
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),

        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),
        glm::vec4(0, 1, 0, 1),

        //up
        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),

        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),

        //down
        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),

        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),
        glm::vec4(0, 0, 1, 1),
    };

    unsigned int num = 36;
};


