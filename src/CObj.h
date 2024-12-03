#include <vector>
#include <cmath>
#include <glm/glm.hpp>

struct Point
{
	glm::vec3 pos;
	glm::vec3 normal;
};

struct Face
{
	int pts[3];
	glm::vec3 normal;
};

class CObj
{
public:
	CObj(void);
	~CObj(void);

	std::vector<Point> m_pts; //顶点
	std::vector<Face> m_faces;//面
	std::vector<unsigned int> indices;//索引

public:
	bool ReadObjFile(const char* pcszFileName);//读入模型文件

private:
	void UnifyModel();//单位化模型
	void ComputeFaceNormal(Face& f);//计算面的法线
	void ComputePointNormal();//计算顶点法线
};

