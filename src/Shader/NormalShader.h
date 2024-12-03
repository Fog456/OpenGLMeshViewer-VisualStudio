
#ifndef MESHVIEWER_NORMALSHADER_H
#define MESHVIEWER_NORMALSHADER_H


//顶点着色器代码
static const char* n_vertex_shader_text =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoords;\n"
"layout (location = 3) in vec3 aTangent;\n"
"layout (location = 4) in vec3 aBitangent;\n"
"\n"
"out VS_OUT {\n"
"    vec3 FragPos;\n"
"    vec2 TexCoords;\n"
"    vec3 TangentLightPos;\n"
"    vec3 TangentViewPos;\n"
"    vec3 TangentFragPos;\n"
"} vs_out;\n"

"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"

"void main()\n"
"{\n"
"    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));   \n"
"    vs_out.TexCoords = aTexCoords;\n"
"    \n"
"    mat3 normalMatrix = transpose(inverse(mat3(model)));\n"
"    vec3 T = normalize(normalMatrix * aTangent);\n"
"    vec3 N = normalize(normalMatrix * aNormal);\n"
"    T = normalize(T - dot(T, N) * N);\n"
"    vec3 B = cross(N, T);\n"
//"    vec4 lightPos_4 = projection * view * model * vec4(lightPos, 0.0)\n"
//"    lightPos = lightPos_4.xyz\n"
"    mat3 TBN = transpose(mat3(T, B, N));    \n"
"    vs_out.TangentLightPos = TBN * lightPos;\n"
"    vs_out.TangentViewPos  = TBN * viewPos;\n"
"    vs_out.TangentFragPos  = TBN * vs_out.FragPos;\n"
"        \n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\n";

//片段着色器代码
static const char* n_fragment_shader_text =
"#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in VS_OUT {\n"
"    vec3 FragPos;\n"
"    vec2 TexCoords;\n"
"    vec3 TangentLightPos;\n"
"    vec3 TangentViewPos;\n"
"    vec3 TangentFragPos;\n"
"} fs_in;\n"
"\n"
"uniform sampler2D diffuseMap;\n"
"uniform sampler2D normalMap;\n"

"void main()\n"
"{           \n"
"     // obtain normal from normal map in range [0,1]\n"
"    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;\n"
"    // transform normal vector to range [-1,1]\n"
"    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space\n"
"   \n"
"    // get diffuse color\n"
"    vec3 color = texture(diffuseMap, fs_in.TexCoords).rgb;\n"

"    // ambient 环境\n"
"    vec3 ambient = 0.1 * color;\n"

"    // diffuse 漫反射\n"
"    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);\n"
"    float diff = max(dot(lightDir, normal), 0.0);\n"
"    vec3 diffuse = diff * color;\n"

"    // specular 镜面\n"
"    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);\n"
"    vec3 reflectDir = reflect(-lightDir, normal);\n"
"    vec3 halfwayDir = normalize(lightDir + viewDir);  \n"
"    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);\n"
"\n"
"    vec3 specular = vec3(0.2) * spec;\n"
"    FragColor = vec4(ambient + diffuse + specular, 1.0);\n"
"}\n";

#endif