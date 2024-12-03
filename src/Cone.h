//#pragma once
//
//#include <glm/glm.hpp>
//
//class Cone
//{
//public:
//    Cone() {
//        //TODO:计算顶点和颜色和数量
//        
//    };
//    glm::vec3* getPos()
//    {
//        return kPositions;
//    }
//    glm::vec4* getColor()
//    {
//        return kColors;
//    }
//    unsigned int getNum()
//    {
//        return num;
//    }
//private:
//    glm::vec3 kPositions[6000];
//    glm::vec4 kColors[6000];
//    unsigned int num;
//};
//
#pragma once

#include <glm/glm.hpp>

class Cone
{
public:
    Cone(unsigned int nSegments = 36, float radius = 1.0f, float height = 2.0f) {
        num = nSegments * 6; // 底面nSegments个顶点，侧面nSegments个顶点，每个三角形3个顶点

        float pi = glm::pi<float>();

        unsigned int index = 0;
        float angleStep = 2.0f * pi / nSegments;
        //glm::vec3 top(0.0f, height, 0.0f); // 锥顶
        //glm::vec3 bottomCenter(0.0f, 0.0f, 0.0f); // 底面圆心
        float halfHeight = height / 2.0f;
        glm::vec3 top(0.0f, halfHeight, 0.0f); // 锥顶
        glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f); // 底面圆心

        glm::vec4 yellow_vec = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);   // 黄色
        glm::vec4 yellow_vec_2 = glm::vec4(1.0f, 0.8f, 0.0f, 1.0f);   // 黄色2
        glm::vec4 magenta_vec = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);  // 品红色

        // 生成底面和侧面的三角形
        for (unsigned int i = 0; i < nSegments; ++i) {
            float theta = i * angleStep;
            float nextTheta = (i + 1) % nSegments * angleStep;

            // 底面的顶点
            glm::vec3 bottomEdge1(radius * cos(theta), -halfHeight, radius * sin(theta));
            glm::vec3 bottomEdge2(radius * cos(nextTheta), -halfHeight, radius * sin(nextTheta));

            // 底面三角形（每个三角形与圆心相连）
            kPositions[index] = bottomCenter;
            kColors[index] = magenta_vec;
            index++;

            kPositions[index] = bottomEdge1;
            kColors[index] = yellow_vec_2;
            index++;

            kPositions[index] = bottomEdge2;
            kColors[index] = yellow_vec_2;
            index++;

            // 侧面的三角形（每个三角形连接锥顶和底面的两点）
            kPositions[index] = top;
            kColors[index] = yellow_vec;
            index++;

            kPositions[index] = bottomEdge2;
            kColors[index] = yellow_vec_2;
            index++;

            kPositions[index] = bottomEdge1;
            kColors[index] = yellow_vec_2;
            index++;
        }
    };

    glm::vec3* getPos() {
        return kPositions;
    }

    glm::vec4* getColor() {
        return kColors;
    }

    unsigned int getNum() {
        return num;
    }

private:
    glm::vec3 kPositions[6000]; // 预留足够的空间用于存储顶点数据
    glm::vec4 kColors[6000];    // 预留足够的空间用于存储颜色数据
    unsigned int num;           // 存储总顶点数
};
