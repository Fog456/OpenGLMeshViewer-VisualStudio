//#pragma once
//
//#include <glm/glm.hpp>
//
//class Cylinder
//{
//public:
//    Cylinder() {
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

class Cylinder
{
public:
    Cylinder(unsigned int nSegments = 36, float radius = 1.0f, float height = 2.0f) {
        num = nSegments * 12; // 顶面nSegments个三角形，底面nSegments个三角形，侧面nSegments * 2个三角形

        float pi = glm::pi<float>();

        unsigned int index = 0;
        float angleStep = 2.0f * pi / nSegments;
        float halfHeight = height / 2.0f;

        // 顶面圆心点
        glm::vec3 topCenter(0.0f, halfHeight, 0.0f);
        // 底面圆心点
        glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f);

        glm::vec4 yellow_vec = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);   // 黄色
        glm::vec4 yellow_vec_2 = glm::vec4(1.0f, 0.9f, 0.0f, 1.0f);   // 黄色
        glm::vec4 magenta_vec = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);  // 品红色

        // 生成顶面和底面的三角形
        for (unsigned int i = 0; i < nSegments; ++i) {
            float theta = i * angleStep;
            float nextTheta = (i + 1) % nSegments * angleStep;

            // 顶面的顶点（与圆心连接）
            glm::vec3 topEdge1(radius * cos(theta), halfHeight, radius * sin(theta));
            glm::vec3 topEdge2(radius * cos(nextTheta), halfHeight, radius * sin(nextTheta));

            // 顶面三角形
            kPositions[index] = topCenter;
            kColors[index] = magenta_vec;
            index++;

            kPositions[index] = topEdge1;
            kColors[index] = yellow_vec_2;
            index++;

            kPositions[index] = topEdge2;
            kColors[index] = yellow_vec_2;
            index++;

            // 底面的顶点（与圆心连接）
            glm::vec3 bottomEdge1(radius * cos(theta), -halfHeight, radius * sin(theta));
            glm::vec3 bottomEdge2(radius * cos(nextTheta), -halfHeight, radius * sin(nextTheta));

            // 底面三角形
            kPositions[index] = bottomCenter;
            kColors[index] = magenta_vec;
            index++;

            kPositions[index] = bottomEdge2;
            kColors[index] = yellow_vec_2;
            index++;

            kPositions[index] = bottomEdge1;
            kColors[index] = yellow_vec_2;
            index++;

            // 侧面三角形（每一块由两个三角形组成）
            // 三角形1
            kPositions[index] = topEdge1;
            kColors[index] = yellow_vec;
            index++;

            kPositions[index] = bottomEdge1;
            kColors[index] = yellow_vec;
            index++;

            kPositions[index] = topEdge2;
            kColors[index] = yellow_vec;
            index++;

            // 三角形2
            kPositions[index] = topEdge2;
            kColors[index] = yellow_vec;
            index++;

            kPositions[index] = bottomEdge1;
            kColors[index] = yellow_vec;
            index++;

            kPositions[index] = bottomEdge2;
            kColors[index] = yellow_vec;
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
