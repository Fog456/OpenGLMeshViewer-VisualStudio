
#ifndef MESHVIEWER_DISPLAYSHADER_H
#define MESHVIEWER_DISPLAYSHADER_H


static const char* d_vertex_shader_text =

"#version 330 core\n"
"layout(location = 0) in vec3 aPos; \n"        // ����λ��
"layout(location = 1) in vec2 aTexCoord; \n"     // ��������
"layout(location = 2) in vec3 aNormal; \n"      // ����

"out vec2 TexCoord; \n"                           // ���ݸ�Ƭ����ɫ������������
"out vec3 FragPos; \n"                            // ���ݸ�Ƭ����ɫ����Ƭ��λ��
"out vec3 Normal; \n"                             // ���ݸ�Ƭ����ɫ���ķ���

"uniform mat4 model; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"

"uniform sampler2D displacementMap; \n"           // �û���ͼ
"uniform float heightScale; \n"                   // �û��߶���������

"void main() { \n"
// ��ȡ��ǰ����������
"TexCoord = aTexCoord; \n"

// ���û���ͼ��ȡ�߶�ֵ
"float displacement = texture(displacementMap, aTexCoord).r; \n" // ȡ��ɫͨ��
"displacement *= heightScale; \n"  // ������������

// �����µĶ���λ��
"vec3 displacedPos = aPos + normalize(aNormal) * displacement; \n" // ����ʹ�÷��߷�������û�

// ��������λ��
"FragPos = displacedPos; \n"
"gl_Position = projection * view * model * vec4(displacedPos, 1.0); \n"
"Normal = mat3(transpose(inverse(model))) * aNormal; \n" // ���㷨��
"}\n";


static const char* d_fragment_shader_text =

"#version 330 core\n"
"out vec4 color; \n"
"in vec2 TexCoord; \n"                       // �Ӷ�����ɫ������������������
"in vec3 FragPos; \n"                        // �Ӷ�����ɫ����������Ƭ��λ��
"in vec3 Normal; \n"                         // �Ӷ�����ɫ���������ķ���

"uniform vec3 lightPos; \n"                  // ��Դλ��
"uniform vec3 viewPos; \n"                   // �۲���λ��
"uniform sampler2D texture1; \n"             // ��������

"void main() { \n"
// �������
"vec3 lightDir = normalize(lightPos - FragPos);\n"
"float diff = max(dot(normalize(Normal), lightDir), 0.0); \n"

// ������ɫ
"vec4 textureColor = texture(texture1, TexCoord); \n"

// ������ɫ
"color = vec4(diff * textureColor.rgb, textureColor.a); \n"
"}\n";

#endif