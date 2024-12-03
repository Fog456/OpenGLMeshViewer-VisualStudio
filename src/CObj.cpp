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
//{//��ȡģ���ļ�
//
//	FILE* fpFile = fopen(pcszFileName, "r"); //��ֻ����ʽ���ļ�
//	if (fpFile == NULL)
//	{
//		return false;
//	}
//
//	m_pts.clear();
//	m_faces.clear();
//	indices.clear();
//
//	//TODO����ģ���ļ��еĵ�������ݷֱ����m_pts��m_faces��
//	
//
//	fclose(fpFile);
//
//	UnifyModel(); //��ģ�͹�һ��
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
    // ��ֻ����ʽ���ļ�
    FILE* fpFile = fopen(pcszFileName, "r");
    if (fpFile == NULL)
    {
        return false; // ����ļ���ʧ�ܣ�����false
    }

    m_pts.clear(); // ������еĶ�������
    m_faces.clear(); // ������е�������
    indices.clear(); // ������е���������

    // ��ȡ�ļ��е�ÿһ��
    char line[128];
    while (fgets(line, 128, fpFile)) {
        // �������"v "��ͷ����ʾ����һ�����㶨��
        if (strncmp(line, "v ", 2) == 0) { 
            Point pt;
            sscanf(line + 2, "%f %f %f", &pt.pos.x, &pt.pos.y, &pt.pos.z);
            m_pts.push_back(pt); // ��������ӵ������б���
        }
        // �������"f "��ͷ����ʾ����һ���涨��
        else if (strncmp(line, "f ", 2) == 0) { 
            Face face;
            int vertexIndices[3];
            sscanf(line + 2, "%d %d %d", &vertexIndices[0], &vertexIndices[1], &vertexIndices[2]);

            // ��������1-basedת��Ϊ0-based
            for (int i = 0; i < 3; ++i) {
                face.pts[i] = vertexIndices[i] - 1;
            }
            m_faces.push_back(face); // ������ӵ����б���

            // ����Ķ���������ӵ������б���
            indices.push_back(face.pts[0]);
            indices.push_back(face.pts[1]);
            indices.push_back(face.pts[2]);
        }
    }

    fclose(fpFile); // �ر��ļ�

    UnifyModel(); // ��һ��ģ�ͣ������ǽ���������ת����һ��ͳһ������ϵ��

    for (int i = 0; i < m_faces.size(); i++)
    {
        ComputeFaceNormal(m_faces[i]); // ����ÿ����ķ���
    }

    ComputePointNormal(); // ����ÿ������ķ���

    return true; // �ɹ���ȡ�ļ����������ݺ󷵻�true
}

void CObj::UnifyModel()
{//Ϊͳһ��ʾ��ͬ�ߴ��ģ�ͣ���ģ�͹�һ������ģ�ͳߴ����ŵ�-1.0-1.0֮��
//ԭ���ҳ�ģ�͵ı߽�������Сֵ�������ҳ�ģ�͵�����
//��ģ�͵����ĵ�Ϊ��׼��ģ�Ͷ����������
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
{//TODO:������f�ķ�������������

    glm::vec3 v1 = m_pts[f.pts[1]].pos - m_pts[f.pts[0]].pos;
    glm::vec3 v2 = m_pts[f.pts[2]].pos - m_pts[f.pts[0]].pos;
    f.normal = glm::normalize(glm::cross(v1, v2));
}

void CObj::ComputePointNormal()
{
    //TODO:���㶥�㷨������������
    for (Point& pt : m_pts) pt.normal = glm::vec3(0.0f);

    for (const Face& f : m_faces) {
        glm::vec3 faceNormal = f.normal;
        for (int i = 0; i < 3; i++) 
            m_pts[f.pts[i]].normal += faceNormal;
    }

    for (Point& pt : m_pts) pt.normal = glm::normalize(pt.normal);
}

