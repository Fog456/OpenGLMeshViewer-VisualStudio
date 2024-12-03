//#pragma once
//
//#include <glm/glm.hpp>
//
//class Cone
//{
//public:
//    Cone() {
//        //TODO:���㶥�����ɫ������
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
        num = nSegments * 6; // ����nSegments�����㣬����nSegments�����㣬ÿ��������3������

        float pi = glm::pi<float>();

        unsigned int index = 0;
        float angleStep = 2.0f * pi / nSegments;
        //glm::vec3 top(0.0f, height, 0.0f); // ׶��
        //glm::vec3 bottomCenter(0.0f, 0.0f, 0.0f); // ����Բ��
        float halfHeight = height / 2.0f;
        glm::vec3 top(0.0f, halfHeight, 0.0f); // ׶��
        glm::vec3 bottomCenter(0.0f, -halfHeight, 0.0f); // ����Բ��

        glm::vec4 yellow_vec = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);   // ��ɫ
        glm::vec4 yellow_vec_2 = glm::vec4(1.0f, 0.8f, 0.0f, 1.0f);   // ��ɫ2
        glm::vec4 magenta_vec = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);  // Ʒ��ɫ

        // ���ɵ���Ͳ����������
        for (unsigned int i = 0; i < nSegments; ++i) {
            float theta = i * angleStep;
            float nextTheta = (i + 1) % nSegments * angleStep;

            // ����Ķ���
            glm::vec3 bottomEdge1(radius * cos(theta), -halfHeight, radius * sin(theta));
            glm::vec3 bottomEdge2(radius * cos(nextTheta), -halfHeight, radius * sin(nextTheta));

            // ���������Σ�ÿ����������Բ��������
            kPositions[index] = bottomCenter;
            kColors[index] = magenta_vec;
            index++;

            kPositions[index] = bottomEdge1;
            kColors[index] = yellow_vec_2;
            index++;

            kPositions[index] = bottomEdge2;
            kColors[index] = yellow_vec_2;
            index++;

            // ����������Σ�ÿ������������׶���͵�������㣩
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
    glm::vec3 kPositions[6000]; // Ԥ���㹻�Ŀռ����ڴ洢��������
    glm::vec4 kColors[6000];    // Ԥ���㹻�Ŀռ����ڴ洢��ɫ����
    unsigned int num;           // �洢�ܶ�����
};
