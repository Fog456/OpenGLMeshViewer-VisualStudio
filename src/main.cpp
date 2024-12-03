#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/transform2.hpp>
#include<glm/gtx/euler_angles.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "Geometry.h"
#include "CObj.h"

#include "Shader/ShaderSource.h"
#include "Shader/ModelShader.h"
#include "Shader/NormalShader.h"
#include "Shader/SkyShader.h"
#include "Shader/DisplayShader.h"
#include "Arcball/arcball.h"

using namespace std;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xposIn, double yposIn);

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
//void processInput(GLFWwindow* window);



int Width = 700, Height = 700;
GLFWwindow* window;
GLuint vertex_shader, fragment_shader, shader, mshader, n_shader, s_shader, b_shader, d_shader;
GLint mvp_location, vpos_location, vcol_location;
GLint m_mvp_location, m_vpos_location, m_vcol_location, m_vnor_location;
GLint light_ambient, light_specular, light_diffuse, light_dir, viewPos, m_model_location;

glm::vec3* pos;
glm::vec4* color;
glm::vec3 cameraPos = glm::vec3(0, 0, 10);
unsigned int num = 0;
int id = 0;

bool lineMode = false;
bool modelTex = false;

// 记录鼠标状态
bool leftButtonPressed = false;
bool rightButtonPressed = false;
float lastX = 400, lastY = 300, lastZ = 0;
float dx = 0.0f, dy = 0.0f;
float pitch = 0.0f, yaw = 0.0f, roll = 0.0f;
bool firstMouse = true;


//坐标系变换
glm::mat4 trans = glm::translate(glm::vec3(0, 0, 0)); //不移动顶点坐标;
glm::mat4 rotation = glm::rotate(glm::radians(0.0f), glm::vec3(0, 0, 1.0f)); //不旋转;
glm::quat q_rot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
float scaleNum = 2.0f;
glm::mat4 scale = glm::scale(glm::vec3(scaleNum, scaleNum, scaleNum)); //缩放;
glm::mat4 model, view, projection;
glm::mat4 mvp;
CArcball arcball = CArcball(Width, Height, Width / 2, Height / 2);

//加载模型
CObj obj;
bool loaded = false;

//纹理
GLuint modelTexture, diffuseMap, normalMap, cubemapTexture;
GLuint diffuseMap_2, normalMap_2, heightMap;

//基础几何图形
Quad* quad = new Quad();
Triangle* triangle = new Triangle();
Cube* cube = new Cube();
Cone* cone = new Cone();
Cylinder* cylinder = new Cylinder();
Wall* wall = new Wall();
SkyBox* skybox = new SkyBox();
Ball* ball = new Ball();


//初始化OpenGL
void init_opengl()
{
    cout << "init opengl" << endl;

    //设置错误回调
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //创建窗口
    window = glfwCreateWindow(Width, Height, "MeshViewer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    gladLoadGL();
    glfwSwapInterval(1);


}

//编译、链接Shader
GLuint compile_shader(const char* vertex, const char* fragment)
{
    //创建顶点Shader
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    //指定Shader源码
    glShaderSource(vertex_shader, 1, &vertex, NULL);
    //编译Shader
    glCompileShader(vertex_shader);
    //获取编译结果
    GLint compile_status = GL_FALSE;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(vertex_shader, sizeof(message), 0, message);
        cout << "compile vs error:" << message << endl;
    }

    //创建片段Shader
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    //指定Shader源码
    glShaderSource(fragment_shader, 1, &fragment, NULL);
    //编译Shader
    glCompileShader(fragment_shader);
    //获取编译结果
    compile_status = GL_FALSE;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        GLchar message[256];
        glGetShaderInfoLog(fragment_shader, sizeof(message), 0, message);
        cout << "compile fs error:" << message << endl;
    }


    //创建GPU程序
    GLuint program = glCreateProgram();
    //附加Shader
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    //Link
    glLinkProgram(program);
    //获取编译结果
    GLint link_status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE)
    {
        GLchar message[256];
        glGetProgramInfoLog(program, sizeof(message), 0, message);
        cout << "link error:" << message << endl;
    }
    return program;
}

void Init_imgui()
{
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 330";
    ImGui_ImplOpenGL3_Init(glsl_version);

}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }


    return textureID;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

bool loadObjFile()
{
    OPENFILENAME ofn;       // 文件对话框结构体
    TCHAR szFile[260];       // 存储文件名的缓冲区

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("OBJ Files(*.OBJ)\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        std::cout << "Selected file: " << szFile << std::endl;
        obj.ReadObjFile(szFile);
        return true;
    }
    else {
        std::cout << "No file selected" << std::endl;
    }

    return false;
}


//TODO
void drawModel()
{
    glm::vec3 lightDir = glm::vec3(0.0f, -1.0f, -1.0f);
    glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);

    // 获取uniform变量在shader程序中的位置
    m_mvp_location = glGetUniformLocation(mshader, "u_mvp"); // 获取模型视图投影矩阵的位置
    m_vpos_location = glGetAttribLocation(mshader, "a_pos"); // 获取顶点位置属性的位置
    m_vnor_location = glGetAttribLocation(mshader, "a_normal"); // 获取顶点法线属性的位置

    light_dir = glGetUniformLocation(mshader, "light.direction");
    light_ambient = glGetUniformLocation(mshader, "light.ambient");
    light_diffuse = glGetUniformLocation(mshader, "light.diffuse");
    light_specular = glGetUniformLocation(mshader, "light.specular");
    viewPos = glGetUniformLocation(mshader, "viewPos");

    m_model_location = glGetUniformLocation(mshader, "model");
    GLint useTex = glGetUniformLocation(mshader, "useTex");

    glUseProgram(mshader);
    glUniformMatrix4fv(m_mvp_location, 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(viewPos, 1, &cameraPos[0]);
    glUniformMatrix4fv(m_model_location, 1, GL_FALSE, &model[0][0]);
    glUniform3fv(light_dir, 1, &lightDir[0]);
    glUniform3fv(light_ambient, 1, &lightAmbient[0]);
    glUniform3fv(light_specular, 1, &lightSpecular[0]);
    glUniform3fv(light_diffuse, 1, &lightDiffuse[0]);
    glUniform1i(useTex, modelTex);

    //TODO:使用索引绘制方式，创建VAO，VBO，EBO
    static GLuint mVAO = 0, mVBO, mEBO;
    //if (mVAO == 0)
    {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, obj.m_pts.size() * sizeof(Point), obj.m_pts.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), obj.indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(m_vpos_location);
        glVertexAttribPointer(m_vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, pos));

        glEnableVertexAttribArray(m_vnor_location);
        glVertexAttribPointer(m_vnor_location, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)offsetof(Point, normal));

    }

    //TODO:绑定贴图
    if (modelTex)
    {
        glActiveTexture(GL_TEXTURE0); // 激活纹理单元0
        glBindTexture(GL_TEXTURE_2D, modelTexture); // 绑定贴图到纹理单元0
        glUniform1i(glGetUniformLocation(mshader, "texture1"), 0); // 将纹理单元传递给着色器
    }


    //绘制
    glBindVertexArray(mVAO);
    glDrawElements(GL_TRIANGLES, obj.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &mVAO);
    glDeleteBuffers(1, &mVBO);
}

void drawNormalTexture()
{
    //TODO:自行创建并编写shader，实现法线贴图效果
    //TIPS:Wall.h中存储了绘制矩形的相关数据

    // 创建 Wall 对象并获取它的顶点数据
    //Wall wall;

    // 创建 VAO/VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定 VAO
    glBindVertexArray(VAO);

    // 绑定并设置 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall->vertices), wall->vertices, GL_STATIC_DRAW);

    // 设置顶点属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));


    glUseProgram(n_shader);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    // 设置光源和视角
    glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, -10.0f); // 替换为光源位置
    glUniform3fv(glGetUniformLocation(n_shader, "lightPos"), 1, &lightPos[0]);
    glUniform3fv(glGetUniformLocation(n_shader, "viewPos"), 1, &cameraPos[0]);
    glUniformMatrix4fv(glGetUniformLocation(n_shader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(n_shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(n_shader, "model"), 1, GL_FALSE, &model[0][0]);

    // 绘制墙面
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制6个顶点（两个三角形）

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

//TODO
void drawSkyBox()
{
    //TODO:自行创建并编写shader，实现环境贴图效果
    //TIPS:ball.h中存储了绘制球体的数据，skybox.h中存储了实现天空盒相关数据
    GLuint ballVAO, ballVBO;

    glGenVertexArrays(1, &ballVAO);
    glGenBuffers(1, &ballVBO);
    glBindVertexArray(ballVAO);
    glBindBuffer(GL_ARRAY_BUFFER, ballVBO);
    glBufferData(GL_ARRAY_BUFFER, (*ball).vertices.size() * sizeof(float), (*ball).vertices.data(), GL_STATIC_DRAW);
    // 设置顶点位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 设置纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 设置法线向量属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 设置切线属性
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // 设置副切线属性
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glUseProgram(b_shader);
    glUniform1i(glGetUniformLocation(b_shader, "skybox"), 0);
    //glUniform3fv(glGetUniformLocation(b_shader, "cameraPos"), 1, &cameraPos[0]);
    glUniformMatrix4fv(glGetUniformLocation(b_shader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(b_shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(b_shader, "model"), 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(ballVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    // 获取 uniform 变量的位置并设置其值
    glDrawElements(GL_TRIANGLES, (*ball).indices.size(), GL_UNSIGNED_INT, (*ball).indices.data());
    glBindVertexArray(0);




    // 创建 skybox 的 VAO 和 VBO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox->vertices), skybox->vertices, GL_STATIC_DRAW);

    // 配置顶点属性
    glEnableVertexAttribArray(0);  // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    // 启用深度测试，将深度函数设置为 GL_LEQUAL 以确保深度为 1.0 的片段正确显示在最前面
    glDepthFunc(GL_LEQUAL);

    // 使用着色器并设置立方体贴图
    glUseProgram(s_shader);
    glUniform1i(glGetUniformLocation(s_shader, "skybox"), 0);
    glUniformMatrix4fv(glGetUniformLocation(s_shader, "projection"), 1, GL_FALSE, &projection[0][0]);
    //glm::vec3 cameraPos = glm::vec3(0, 0, 0);
    glm::mat4 view = glm::mat4(glm::mat3(glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))));
    glUniformMatrix4fv(glGetUniformLocation(s_shader, "view"), 1, GL_FALSE, &view[0][0]);
    float scaleNum = 5.0f; glm::mat4 scale = glm::scale(glm::vec3(scaleNum, scaleNum, scaleNum)); //缩放;
    glUniformMatrix4fv(glGetUniformLocation(s_shader, "scale"), 1, GL_FALSE, &scale[0][0]);


    // 绑定天空盒纹理
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);


    // 绘制天空盒立方体
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 恢复深度测试模式
    glDepthFunc(GL_LESS);

    // 释放资源
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &ballVAO);
    glDeleteBuffers(1, &ballVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
}
//TODO
void drawDisplacement2()
{
    //TODO:自行创建并编写shader，实现置换贴图效果
    //TIPS:ball.h中存储了绘制球体的数据

}

void drawDisplacement()
{
    //TODO:自行创建并编写shader，实现置换贴图效果
    //TIPS:ball.h中存储了绘制球体的数据
    glm::vec3 lightPos = glm::vec3(0.0f, -1.0f, -1.0f);
    float heightScale = 1.0f;

    glUseProgram(d_shader);

    glDepthFunc(GL_LEQUAL);
    glUniformMatrix4fv(glGetUniformLocation(d_shader, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(d_shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(d_shader, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3fv(glGetUniformLocation(d_shader, "viewPos"), 1, &cameraPos[0]);
    glUniform3fv(glGetUniformLocation(d_shader, "lightPos"), 1, &cameraPos[0]);

    GLint heightScaleLoc = glGetUniformLocation(d_shader, "heightScale");
    // 设置 uniform 变量的值
    glUniform1f(heightScaleLoc, heightScale);

    GLint skyboxLoc = glGetUniformLocation(d_shader, "diffuseMap");
    glUniform1i(skyboxLoc, 0);
    skyboxLoc = glGetUniformLocation(d_shader, "normalMap");
    glUniform1i(skyboxLoc, 1);
    skyboxLoc = glGetUniformLocation(d_shader, "depthMap");
    glUniform1i(skyboxLoc, 2);


    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (*ball).vertices.size() * sizeof(float), (*ball).vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*ball).indices.size() * sizeof(unsigned int), (*ball).indices.data(), GL_STATIC_DRAW);

    // 设置顶点位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 设置纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 设置法线向量属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    // 设置切线属性
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);
    // 设置副切线属性
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    glEnableVertexAttribArray(4);


    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap_2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap_2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, heightMap);
    glDrawElements(GL_TRIANGLES, (*ball).indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

int main(void)
{
    init_opengl();

    shader = compile_shader(vertex_shader_text, fragment_shader_text);
    mshader = compile_shader(mvertex_shader_text, mfragment_shader_text);
    n_shader = compile_shader(n_vertex_shader_text, n_fragment_shader_text);
    s_shader = compile_shader(s_vertex_shader_text, s_fragment_shader_text);
    b_shader = compile_shader(b_vertex_shader_text, b_fragment_shader_text);
    d_shader = compile_shader(d_vertex_shader_text, d_fragment_shader_text);

    //TODO:载入需要的贴图
    //TIPS:使用loadTexture和loadCubemap函数
    modelTexture = loadTexture("../../../resources/textures/brickwall.jpg");

    diffuseMap = loadTexture("../../../resources/textures/brickwall.jpg");
    normalMap = loadTexture("../../../resources/textures/brickwall_normal.jpg");

    cubemapTexture = loadCubemap(skybox->faces);

    diffuseMap_2 = loadTexture("../../../resources/textures/Ground054_1K-JPG/Ground054_1K-JPG_Color.jpg");
    normalMap_2 = loadTexture("../../../resources/textures/Ground054_1K-JPG/Ground054_1K-JPG_NormalDX.jpg");
    heightMap = loadTexture("../../../resources/textures/Ground054_1K-JPG/Ground054_1K-JPG_Roughness.jpg");

    Init_imgui();

    //获取shader属性ID
    mvp_location = glGetUniformLocation(shader, "u_mvp");
    vpos_location = glGetAttribLocation(shader, "a_pos");
    vcol_location = glGetAttribLocation(shader, "a_color");


    while (!glfwWindowShouldClose(window))
    {
        cameraPos = glm::vec3(0, 0, 10);

        if (lineMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        float ratio;
        //获取画面宽高
        glfwGetFramebufferSize(window, &Width, &Height);
        ratio = Width / (float)Height;

        glViewport(0, 0, Width, Height);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f / 255, 77.f / 255, 121.f / 255, 1.f);

        //根据 gui选项 设置 id，加载数据
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Setup");

            ImGui::Checkbox("Line Mode", &lineMode);
            ImGui::Checkbox("ModelTexture", &modelTex);// Edit bools storing our window open/close state

            if (ImGui::Button("Triangle"))
                id = 0;
            if (ImGui::Button("Quad"))
                id = 1;
            if (ImGui::Button("Cube"))
                id = 2;
            if (ImGui::Button("Cone"))
                id = 3;
            if (ImGui::Button("Cylinder"))
                id = 4;
            if (ImGui::Button("Model"))
            {
                id = 5;
                loaded = loadObjFile();
            }
            if (ImGui::Button("NormalTex"))
                id = 6;
            if (ImGui::Button("SkyBox"))
                id = 7;
            if (ImGui::Button("Displacement"))
                id = 8;

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();

            //cout << "id: " << id << endl;
        }
        switch (id)
        {
        case 0:
            pos = triangle->getPos();
            color = triangle->getColor();
            num = triangle->getNum();
            break;
        case 1:
            pos = quad->getPos();
            color = quad->getColor();
            num = quad->getNum();
            break;
        case 2:
            pos = cube->getPos();
            color = cube->getColor();
            num = cube->getNum();
            break;
        case 3:
            pos = cone->getPos();
            color = cone->getColor();
            num = cone->getNum();
            break;
        case 4:
            pos = cylinder->getPos();
            color = cylinder->getColor();
            num = cylinder->getNum();
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            break;
        default:
            cout << "wrong" << endl;
        }

        //坐标系变换
        scale = glm::scale(glm::vec3(scaleNum, scaleNum, scaleNum)); //缩放;

        model = trans * scale * rotation;

        view = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        projection = glm::perspective(glm::radians(60.f), ratio, 1.f, 1000.f);

        mvp = projection * view * model;

        //根据 id 绘制
        switch (id)
        {
            //0 三角形、1 正方形、2 正方体、3 圆锥、4 圆柱
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            //指定GPU程序(就是指定顶点着色器、片段着色器)
            glUseProgram(shader);
            glEnable(GL_DEPTH_TEST);

            //启用顶点Shader属性(a_pos)，指定与顶点坐标数据进行关联
            glEnableVertexAttribArray(vpos_location);
            glVertexAttribPointer(vpos_location, 3, GL_FLOAT, false, sizeof(glm::vec3), pos);

            //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联
            glEnableVertexAttribArray(vcol_location);
            glVertexAttribPointer(vcol_location, 3, GL_FLOAT, false, sizeof(glm::vec4), color);

            //上传mvp矩阵
            glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
            //上传顶点数据并进行绘制
            glDrawArrays(GL_TRIANGLES, 0, num);

            glBindVertexArray(0);


            //cout << "id" << id << endl;
            break;
        case 5:
            if (loaded) drawModel();
            break;
        case 6:
            drawNormalTexture();
            break;
        case 7:
            drawSkyBox();
            break;
        case 8:
            drawDisplacement();
            break;
        default:
            cout << "wrong" << endl;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//鼠标移动回调函数
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //TODO:通过鼠标操作实现旋转，平移
    //TIPS:修改平移，旋转矩阵

    if (firstMouse)
    {
        lastX = xposIn;
        lastY = yposIn;
        firstMouse = false;
    }


    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // 计算鼠标的位移
    float deltaX = xpos - lastX;
    float deltaY = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    // 左键控制平移
    if (leftButtonPressed) {
        float sensitivity = 0.01f; // 平移敏感度
        trans = glm::translate(trans, glm::vec3(deltaX * sensitivity, -deltaY * sensitivity, 0.0f));
    }

    // 右键控制旋转
    if (rightButtonPressed) {
        float sensitivity = 0.005f; // 旋转敏感度
        rotation = glm::rotate(rotation, deltaX * sensitivity, glm::vec3(0.0f, 1.0f, 0.0f)); // 水平旋转
        rotation = glm::rotate(rotation, -deltaY * sensitivity, glm::vec3(1.0f, 0.0f, 0.0f)); // 垂直旋转

        //// 鼠标右键按住，执行旋转
        //float sensitivity = 0.1f; // 调整旋转灵敏度
        //yaw += deltaX * sensitivity;
        //pitch += deltaY * sensitivity;

        //// 更新旋转矩阵
        //glm::mat4 rotationX = glm::rotate(glm::radians(pitch), glm::vec3(1, 0, 0));
        //glm::mat4 rotationY = glm::rotate(glm::radians(yaw), glm::vec3(0, 1, 0));
        //rotation = rotationY * rotationX; // 更新旋转矩阵

    }

}

// 鼠标按键回调函数，用于记录鼠标按下状态
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS)
            leftButtonPressed = true;
        else if (action == GLFW_RELEASE)
            leftButtonPressed = false;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS)
            rightButtonPressed = true;
        else if (action == GLFW_RELEASE)
            rightButtonPressed = false;
    }
}


void scroll_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    //TODO：鼠标滚轮操作，修改scaleNum变量以实现缩放
    scaleNum += (float)(yposIn * (scaleNum / 5.0));

    float min_scaleNum = 0.5f, max_scaleNum = 10.0f;
    if (scaleNum < min_scaleNum)
        scaleNum = min_scaleNum;
    if (scaleNum > max_scaleNum)
        scaleNum = max_scaleNum;
}