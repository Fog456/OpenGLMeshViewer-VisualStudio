#include "CObj.h"
#include <iostream>
#include <sstream>
#include <algorithm>

CObj::CObj(void)
{
}


CObj::~CObj(void)
{
}

//bool CObj::ReadObjFile(const char* pcszFileName)
//{//读取模型文件
//
//	FILE* fpFile = fopen(pcszFileName, "r"); //以只读方式打开文件
//	if (fpFile == NULL)
//	{
//		return false;
//	}
//
//	m_pts.clear();
//	m_faces.clear();
//	indices.clear();
//
//	//TODO：将模型文件中的点和面数据分别存入m_pts和m_faces中
//	
//
//	fclose(fpFile);
//
//	UnifyModel(); //将模型归一化
//	for (int i = 0; i < m_faces.size(); i++)
//	{
//		ComputeFaceNormal(m_faces[i]);
//	}
//	ComputePointNormal();
//	
//
//	return true;
//}
bool CObj::ReadObjFile(const char* pcszFileName)
{
    // 以只读方式打开文件
    FILE* fpFile = fopen(pcszFileName, "r");
    if (fpFile == NULL)
    {
        return false; // 如果文件打开失败，返回false
    }

    m_pts.clear(); // 清除现有的顶点数据
    m_faces.clear(); // 清除现有的面数据
    indices.clear(); // 清除现有的索引数据

    // 读取文件中的每一行
    char line[128];
    while (fgets(line, 128, fpFile)) {
        // 如果行以"v "开头，表示这是一个顶点定义
        if (strncmp(line, "v ", 2) == 0) { 
            Point pt;
            sscanf(line + 2, "%f %f %f", &pt.pos.x, &pt.pos.y, &pt.pos.z);
            m_pts.push_back(pt); // 将顶点添加到顶点列表中
        }
        // 如果行以"f "开头，表示这是一个面定义
        else if (strncmp(line, "f ", 2) == 0) { 
            Face face;
            int vertexIndices[3];
            sscanf(line + 2, "%d %d %d", &vertexIndices[0], &vertexIndices[1], &vertexIndices[2]);

            // 将索引从1-based转换为0-based
            for (int i = 0; i < 3; ++i) {
                face.pts[i] = vertexIndices[i] - 1;
            }
            m_faces.push_back(face); // 将面添加到面列表中

            // 将面的顶点索引添加到索引列表中
            indices.push_back(face.pts[0]);
            indices.push_back(face.pts[1]);
            indices.push_back(face.pts[2]);
        }
    }

    fclose(fpFile); // 关闭文件

    UnifyModel(); // 归一化模型，可能是将顶点坐标转换到一个统一的坐标系中

    for (int i = 0; i < m_faces.size(); i++)
    {
        ComputeFaceNormal(m_faces[i]); // 计算每个面的法线
    }

    ComputePointNormal(); // 计算每个顶点的法线

    return true; // 成功读取文件并处理数据后返回true
}

void CObj::UnifyModel()
{//为统一显示不同尺寸的模型，将模型归一化，将模型尺寸缩放到-1.0-1.0之间
//原理：找出模型的边界最大和最小值，进而找出模型的中心
//以模型的中心点为基准对模型顶点进行缩放
    glm::vec3 minPt(FLT_MAX), maxPt(-FLT_MAX);
    for (const Point& pt : m_pts) {
        minPt = glm::min(minPt, pt.pos);
        maxPt = glm::max(maxPt, pt.pos);
    }
    glm::vec3 center = (minPt + maxPt) / 2.0f;
    float scaleFactor = 2.0f / glm::max(glm::length(maxPt - center), 1.0f);

    int i = 0;
    for (Point& pt : m_pts) {
        pt.pos = (pt.pos - center) * scaleFactor;
    }
}

void CObj::ComputeFaceNormal(Face& f)
{//TODO:计算面f的法向量，并保存

    glm::vec3 v1 = m_pts[f.pts[1]].pos - m_pts[f.pts[0]].pos;
    glm::vec3 v2 = m_pts[f.pts[2]].pos - m_pts[f.pts[0]].pos;
    f.normal = glm::normalize(glm::cross(v1, v2));
}

void CObj::ComputePointNormal()
{
    //TODO:计算顶点法向量，并保存
    for (Point& pt : m_pts) pt.normal = glm::vec3(0.0f);

    for (const Face& f : m_faces) {
        glm::vec3 faceNormal = f.normal;
        for (int i = 0; i < 3; i++) 
            m_pts[f.pts[i]].normal += faceNormal;
    }

    for (Point& pt : m_pts) pt.normal = glm::normalize(pt.normal);
}

