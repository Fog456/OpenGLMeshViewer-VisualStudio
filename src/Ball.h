#pragma once

#include <glm/glm.hpp>

class Ball
{
public:
    Ball() {
        //计算顶点
        const float PI = 3.14159265358979323846f;
        const unsigned int X_SEGMENTS = 64;  // 经度细分
        const unsigned int Y_SEGMENTS = 64;  // 纬度细分
        const float radius = 1.0f;           // 球的半径

        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                // 计算球面坐标
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = radius * std::cos(ySegment * PI);
                float zPos = radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                // 顶点位置
                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);

                // 纹理坐标
                vertices.push_back(xSegment);
                vertices.push_back(ySegment);

                // 法线向量（与顶点位置相同，因为球的法线指向球心）
                vertices.push_back(xPos);
                vertices.push_back(yPos);
                vertices.push_back(zPos);

                // 切线和副切线 (先占位, 后续计算)
                vertices.push_back(0.0f); // Tangent X
                vertices.push_back(0.0f); // Tangent Y
                vertices.push_back(0.0f); // Tangent Z
                vertices.push_back(0.0f); // Bitangent X
                vertices.push_back(0.0f); // Bitangent Y
                vertices.push_back(0.0f); // Bitangent Z
            }
        }

        // 生成三角形索引
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
            for (unsigned int x = 0; x < X_SEGMENTS; ++x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x + 1);

                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
            }
        }

        // 计算切线和副切线
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            unsigned int i1 = indices[i];
            unsigned int i2 = indices[i + 1];
            unsigned int i3 = indices[i + 2];

            // 顶点位置
            glm::vec3 pos1(vertices[i1 * 14 + 0], vertices[i1 * 14 + 1], vertices[i1 * 14 + 2]);
            glm::vec3 pos2(vertices[i2 * 14 + 0], vertices[i2 * 14 + 1], vertices[i2 * 14 + 2]);
            glm::vec3 pos3(vertices[i3 * 14 + 0], vertices[i3 * 14 + 1], vertices[i3 * 14 + 2]);

            // 纹理坐标
            glm::vec2 uv1(vertices[i1 * 14 + 3], vertices[i1 * 14 + 4]);
            glm::vec2 uv2(vertices[i2 * 14 + 3], vertices[i2 * 14 + 4]);
            glm::vec2 uv3(vertices[i3 * 14 + 3], vertices[i3 * 14 + 4]);

            // 计算边和纹理差值
            glm::vec3 deltaPos1 = pos2 - pos1;
            glm::vec3 deltaPos2 = pos3 - pos1;
            glm::vec2 deltaUV1 = uv2 - uv1;
            glm::vec2 deltaUV2 = uv3 - uv1;

            // 计算切线和副切线
            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
            glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

            // 更新顶点数据中的切线和副切线
            vertices[i1 * 14 + 8] += tangent.x;
            vertices[i1 * 14 + 9] += tangent.y;
            vertices[i1 * 14 + 10] += tangent.z;
            vertices[i1 * 14 + 11] += bitangent.x;
            vertices[i1 * 14 + 12] += bitangent.y;
            vertices[i1 * 14 + 13] += bitangent.z;

            vertices[i2 * 14 + 8] += tangent.x;
            vertices[i2 * 14 + 9] += tangent.y;
            vertices[i2 * 14 + 10] += tangent.z;
            vertices[i2 * 14 + 11] += bitangent.x;
            vertices[i2 * 14 + 12] += bitangent.y;
            vertices[i2 * 14 + 13] += bitangent.z;

            vertices[i3 * 14 + 8] += tangent.x;
            vertices[i3 * 14 + 9] += tangent.y;
            vertices[i3 * 14 + 10] += tangent.z;
            vertices[i3 * 14 + 11] += bitangent.x;
            vertices[i3 * 14 + 12] += bitangent.y;
            vertices[i3 * 14 + 13] += bitangent.z;
        }
        
        // 最后对切线和副切线进行归一化
        for (unsigned int i = 0; i < vertices.size(); i += 14) {
            glm::vec3 tangent(vertices[i + 8], vertices[i + 9], vertices[i + 10]);
            glm::vec3 bitangent(vertices[i + 11], vertices[i + 12], vertices[i + 13]);

            tangent = glm::normalize(tangent);
            bitangent = glm::normalize(bitangent);

            vertices[i + 8] = tangent.x;
            vertices[i + 9] = tangent.y;
            vertices[i + 10] = tangent.z;
            vertices[i + 11] = bitangent.x;
            vertices[i + 12] = bitangent.y;
            vertices[i + 13] = bitangent.z;
        }
    };

public:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int num;
};

