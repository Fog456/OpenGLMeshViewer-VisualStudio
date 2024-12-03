
#ifndef MESHVIEWER_DISPLAYSHADER_H
#define MESHVIEWER_DISPLAYSHADER_H


static const char* d_vertex_shader_text =

"#version 330 core\n"
"layout(location = 0) in vec3 aPos; \n"        // 顶点位置
"layout(location = 1) in vec2 aTexCoord; \n"     // 纹理坐标
"layout(location = 2) in vec3 aNormal; \n"      // 法线

"out vec2 TexCoord; \n"                           // 传递给片段着色器的纹理坐标
"out vec3 FragPos; \n"                            // 传递给片段着色器的片段位置
"out vec3 Normal; \n"                             // 传递给片段着色器的法线

"uniform mat4 model; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"

"uniform sampler2D displacementMap; \n"           // 置换贴图
"uniform float heightScale; \n"                   // 置换高度缩放因子

"void main() { \n"
// 获取当前的纹理坐标
"TexCoord = aTexCoord; \n"

// 从置换贴图获取高度值
"float displacement = texture(displacementMap, aTexCoord).r; \n" // 取红色通道
"displacement *= heightScale; \n"  // 乘以缩放因子

// 计算新的顶点位置
"vec3 displacedPos = aPos + normalize(aNormal) * displacement; \n" // 假设使用法线方向进行置换

// 计算最终位置
"FragPos = displacedPos; \n"
"gl_Position = projection * view * model * vec4(displacedPos, 1.0); \n"
"Normal = mat3(transpose(inverse(model))) * aNormal; \n" // 计算法线
"}\n";


static const char* d_fragment_shader_text =

"#version 330 core\n"
"out vec4 color; \n"
"in vec2 TexCoord; \n"                       // 从顶点着色器传递来的纹理坐标
"in vec3 FragPos; \n"                        // 从顶点着色器传递来的片段位置
"in vec3 Normal; \n"                         // 从顶点着色器传递来的法线

"uniform vec3 lightPos; \n"                  // 光源位置
"uniform vec3 viewPos; \n"                   // 观察者位置
"uniform sampler2D texture1; \n"             // 基础纹理

"void main() { \n"
// 计算光照
"vec3 lightDir = normalize(lightPos - FragPos);\n"
"float diff = max(dot(normalize(Normal), lightDir), 0.0); \n"

// 基础颜色
"vec4 textureColor = texture(texture1, TexCoord); \n"

// 最终颜色
"color = vec4(diff * textureColor.rgb, textureColor.a); \n"
"}\n";

#endif