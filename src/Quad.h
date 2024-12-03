#pragma once

#include <glm/glm.hpp>

class Quad
{
public:
    Quad() {};
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
    glm::vec3 kPositions[6] = {
        //第一个三角形
        { -1.0f, -1.0f,0.0f},//左下
        {  1.0f, -1.0f,0.0f},//右下
        {  1.0f,  1.0f,0.0f},//右上

        //第二个三角形
        {  1.0f,  1.0f,0.0f},//右上
        { -1.0f,  1.0f,0.0f},//左上
        { -1.0f, -1.0f,0.0f} //左下
    };
    glm::vec4 kColors[6] = {
        //第一个三角形颜色
        { 1.f, 0.f, 0.f ,1.f},//左下
        { 0.f, 1.f, 0.f ,1.f},//右下
        { 0.f, 0.f, 1.f ,1.f},//右上

        //第二个三角形颜色
        { 0.f, 0.f, 1.f ,1.f},//右上
        { 1.f, 0.f, 0.f ,1.f},//左上
        { 1.f, 0.f, 0.f ,1.f} //左下
    };
    unsigned int num = 6;
};

