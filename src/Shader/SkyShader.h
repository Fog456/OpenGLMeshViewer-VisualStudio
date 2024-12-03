

#ifndef MESHVIEWER_SKYSHADER_H
#define MESHVIEWER_SKYSHADER_H


//顶点着色器代码
static const char* s_vertex_shader_text =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"\n"
"out vec3 TexCoords;\n"
"\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 scale;\n"
"\n"
"void main()\n"
"{\n"
"    TexCoords = aPos;\n"
"    vec4 pos = projection * view * scale * vec4(aPos, 1.0);\n"
"    gl_Position = pos.xyww;\n"
"}  \n";

//片段着色器代码
static const char* s_fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 TexCoords;\n"
"\n"
"uniform samplerCube skybox;\n"
"\n"
"void main()\n"
"{    \n"
"    FragColor = texture(skybox, TexCoords);\n"
"}\n";


//顶点着色器代码
static const char* b_vertex_shader_text =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"\n"
"out vec3 Normal;\n"
"out vec3 Position;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    Position = vec3(model * vec4(aPos, 1.0));\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

//片段着色器代码
static const char* b_fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 Normal;\n"
"in vec3 Position;\n"
"\n"
"uniform vec3 cameraPos;\n"
"uniform samplerCube skybox;\n"
"\n"
"void main()\n"
"{\n"
"    vec3 I = normalize(Position - cameraPos);\n"
"    vec3 R = reflect(I, normalize(Normal));\n"
"    FragColor = vec4(texture(skybox, R).rgb, 1.0);\n"
"}\n";


#endif