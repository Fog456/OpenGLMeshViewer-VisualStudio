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
#include <filesystem>
namespace fs = std::filesystem;

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "Geometry.h"
#include "CObj.h"

#include "Shader/ShaderSource.h"
#include "Shader/ModelShader.h"
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
GLuint vertex_shader, fragment_shader, shader, mshader, mshader_2;
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
GLuint modelTexture, diffuseMap, normalMap;

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
    //TODO:使用索引绘制方式，创建VAO，VBO，EBO


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
    //unsigned int modelTextureID = loadTexture("../resources/textures/bricks2.jpg"); 
    //const unsigned int modelTextureID = loadTexture("../resources/textures/brickwall.jpg");
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

//TODO
void drawNormalTexture()
{
    //TODO:自行创建并编写shader，实现法线贴图效果
    //TIPS:Wall.h中存储了绘制矩形的相关数据

    // 初始化着色器
    /*GLuint shaderProgram = compile_shader(mvertex_shader_text, mfragment_shader_text);*/
    glUseProgram(mshader_2);

    // 设置光照参数
    GLint lightDirLoc = glGetUniformLocation(mshader_2, "light.direction");
    glUniform3f(lightDirLoc, -0.2f, -1.0f, -0.3f);
    GLint lightAmbientLoc = glGetUniformLocation(mshader_2, "light.ambient");
    glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
    GLint lightDiffuseLoc = glGetUniformLocation(mshader_2, "light.diffuse");
    glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
    GLint lightSpecularLoc = glGetUniformLocation(mshader_2, "light.specular");
    glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

    // 加载纹理和法线贴图
    /*GLuint diffuseMap = loadTexture("../resources/textures/brickwall.jpg");
    GLuint normalMap = loadTexture("../resources/textures/brickwall_normal.jpg");*/


    glUniform1i(glGetUniformLocation(mshader_2, "chessTex"), 0);
    glUniform1i(glGetUniformLocation(mshader_2, "normalMap"), 1);

    // 激活纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);

    // 创建 VAO 和 VBO
    GLuint nVAO = 0, nVBO;
    if (nVAO == 0)
    {
        glGenVertexArrays(1, &nVAO);
        glGenBuffers(1, &nVBO);

        glBindVertexArray(nVAO);
        glBindBuffer(GL_ARRAY_BUFFER, nVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(wall->vertices), wall->vertices, GL_STATIC_DRAW);
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
    }

    glBindVertexArray(nVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);


}
//TODO
void drawSkyBox()
{
    //TODO:自行创建并编写shader，实现环境贴图效果
    //TIPS:ball.h中存储了绘制球体的数据，skybox.h中存储了实现天空盒相关数据

}
//TODO
void drawDisplacement()
{
    //TODO:自行创建并编写shader，实现置换贴图效果
    //TIPS:ball.h中存储了绘制球体的数据


}


int main(void)
{
    init_opengl();

    shader = compile_shader(vertex_shader_text, fragment_shader_text);
    mshader = compile_shader(mvertex_shader_text, mfragment_shader_text);
    mshader_2 = compile_shader(n_vertex_shader_text, n_fragment_shader_text);

    //TODO:载入需要的贴图
    //TIPS:使用loadTexture和loadCubemap函数
    diffuseMap = loadTexture("../../../resources/textures/bricks2.jpg");
    normalMap = loadTexture("../../../resources/textures/brickwall_normal.jpg");
    modelTexture = loadTexture("../../../resources/textures/brickwall.jpg");

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
    }

}
//void mouse_callback2(GLFWwindow* window, double xposIn, double yposIn) {
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    // 处理初始鼠标位置
//    if (firstMouse) {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    // 计算鼠标位移
//    dx = xpos - lastX;
//    dy = ypos - lastY;
//    lastX = xpos;
//    lastY = ypos;
//
//    // 平移
//    if (leftButtonPressed) {
//        glm::vec3 translation = glm::vec3(dx * 0.01f, -dy * 0.01f, 0.0f);
//        trans = glm::translate(trans, translation);
//    }
//
//    //// 旋转
//    //if (rightButtonPressed) {
//    //    CQrot q_update = arcball.update(static_cast<int>(xpos), static_cast<int>(ypos));
//    //    glm::quat q_delta(q_update.w, q_update.x, q_update.y, q_update.z);
//    //    q_rot = q_delta * q_rot;
//    //    q_rot = glm::normalize(q_rot);
//
//    //    // 将四元数转换为旋转矩阵
//    //    //rotation = glm::toMat4(q_rot);
//    //    rotation = glm::toMat4(q_rot);
//    //}
//
//    // 旋转
//    if (rightButtonPressed) {
//        // 使用 Arcball 获取新的旋转四元数
//        CQrot newRotation = arcball.update(static_cast<int>(xpos), static_cast<int>(ypos));
//
//        // 将新的旋转应用到当前四元数旋转 q_rot
//        q_rot = newRotation * q_rot;
//        q_rot.normalize();
//
//        // 将四元数转换为旋转矩阵
//        double rotationMatrix[16];
//        q_rot.convert(rotationMatrix);
//
//        // 更新旋转矩阵
//        rotation = glm::make_mat4(rotationMatrix);
//    }
//}

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