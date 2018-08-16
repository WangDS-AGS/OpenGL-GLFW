#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <learnopengl/filesystem.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/camera.h>

//===================macro & header process start===================
//triangle draw test
#define DOUBLE_TRI_DRAW 0
#define UNIFORM_DRAW 0
#define BLEND_DRAW 0
#define OFFSET_DRAW 0
#define BLEND_DRAW_ENCAPSOLUTION 0

//texture box test
#define TEXTURE_BOX 0
#define TEXTURE_BOX_AND_WINDOW 0
#define TEXTURE_MATRIX_OP 0

//coordinate system test
#define COORDINATE_SYSTEM 0
#define COORDINATE_SYSTEM_CUBE_ROTATE 0
#define COORDINATE_SYSTEM_MULTI_CUBE_ROTATE 0

//camera view test
#define CAMERA_VIEW 0
#define CAMERA_VIEW_INPUT 0
#define CAMERA_VIEW_MOUSE_AND_ZOOM 1
#define USE_CAMERA_CLASS 1  // use /not use camera class

#define SHADER_ROOT_DIR "/Users/wangdaosheng/Desktop/X-Project/Xcode_Source/openGLtest1/shader/"

#if COORDINATE_SYSTEM | COORDINATE_SYSTEM_CUBE_ROTATE | COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | \
    CAMERA_VIEW | CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
#include <learnopengl/shader_m.h>
#else
#include <learnopengl/shader_s.h>
#endif
//===================macro & header process end===================

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

#if CAMERA_VIEW_MOUSE_AND_ZOOM
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
#endif

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#if CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;
#endif

#if CAMERA_VIEW_MOUSE_AND_ZOOM

#if USE_CAMERA_CLASS
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
#else
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
#endif

bool firstMouse = true;
float yaw   = -90.0f;    // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float fov   =  45.0f;
#endif

//===================vertex shader programme defination start===================
#if DOUBLE_TRI_DRAW | UNIFORM_DRAW
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
#endif

#if BLEND_DRAW
const char *vertexShaderBlendSource ="#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";
#endif

#if OFFSET_DRAW
const char* vertexShaderOffsetSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n" //顶点数据传入的坐标
"layout (location = 1) in vec3 color;\n"    //顶点数据传入的颜色
"uniform vec4 offset;\n"
"out vec3 glColor;\n"
"out vec4 vertexColor;\n"   //将顶点坐标作为颜色传入片段着色器，测试所得效果
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, -position.y, position.z, 1.0)+offset;\n"
"vertexColor=gl_Position;\n"
"glColor=color;\n"
"}\0";
#endif

//fragment shader defination
#if DOUBLE_TRI_DRAW
const char *fragmentShaderOrangeSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

const char *fragmentShaderYellowSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\n\0";
#endif

#if UNIFORM_DRAW
const char *fragmentUniformShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";
#endif

#if BLEND_DRAW
const char *fragmentShaderBlendSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";
#endif

#if OFFSET_DRAW
const char* fragmentShaderOffsetSource = "#version 330 core\n"
"out vec4 color;\n"
"in vec4 vertexColor;\n"
"in vec3 glColor;\n"
"void main()\n"
"{\n"
"color = vec4(glColor, 1.0f);\n"//将顶点颜色和坐标转换的颜色进行混合
//"color = vec4(glColor, 1.0f)+vertexColor;\n"//将顶点颜色和坐标转换的颜色进行混合
"}\n\0";
#endif
//===================vertex shader programme defination end===================

int main()
{
    // @1.1 glfw: initialize and configure---------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    
    // @1.2 glfw window creation---------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "DaoshengOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

#if CAMERA_VIEW_MOUSE_AND_ZOOM
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
    
    // @1.3 glad: load all OpenGL function pointers---------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // @2 build and compile our shader program---------------------
#if DOUBLE_TRI_DRAW | UNIFORM_DRAW | BLEND_DRAW | OFFSET_DRAW
    int success;
    char infoLog[512];
#endif
    
#if DOUBLE_TRI_DRAW |UNIFORM_DRAW
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
#endif
    
#if BLEND_DRAW
    // vertex blend shader
    int vertexBlendShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexBlendShader, 1, &vertexShaderBlendSource, NULL);
    glCompileShader(vertexBlendShader);
    // check for shader compile errors
    glGetShaderiv(vertexBlendShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexBlendShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
#endif

#if OFFSET_DRAW
    // vertex offset shader
    int vertexOffsetShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexOffsetShader, 1, &vertexShaderOffsetSource, NULL);
    glCompileShader(vertexOffsetShader);
    // check for shader compile errors
    glGetShaderiv(vertexOffsetShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexOffsetShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
#endif

#if DOUBLE_TRI_DRAW
    // fragment orange shader
    int fragmentOrangeShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentOrangeShader, 1, &fragmentShaderOrangeSource, NULL);
    glCompileShader(fragmentOrangeShader);
    // check for shader compile errors
    glGetShaderiv(fragmentOrangeShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentOrangeShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // fragment yellow shader
    int fragmentYellowShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentYellowShader, 1, &fragmentShaderYellowSource, NULL);
    glCompileShader(fragmentYellowShader);
    // check for shader compile errors
    glGetShaderiv(fragmentYellowShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentYellowShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT1::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
#endif

#if UNIFORM_DRAW
    // fragment uninform shader
    int fragmentUniformShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentUniformShader, 1, &fragmentUniformShaderSource, NULL);
    glCompileShader(fragmentUniformShader);
    // check for shader compile errors
    glGetShaderiv(fragmentUniformShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentUniformShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
#endif
    
#if BLEND_DRAW
    // fragment blend shader
    int fragmentBlendShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentBlendShader, 1, &fragmentShaderBlendSource, NULL);
    glCompileShader(fragmentBlendShader);
    // check for shader compile errors
    glGetShaderiv(fragmentBlendShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentBlendShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
#endif
    
#if OFFSET_DRAW
    // fragment offset shader
    int fragmentOffsetShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentOffsetShader, 1, &fragmentShaderOffsetSource, NULL);
    glCompileShader(fragmentOffsetShader);
    // check for shader compile errors
    glGetShaderiv(fragmentOffsetShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentOffsetShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
#endif

#if DOUBLE_TRI_DRAW
    // link Orange shaders
    int shaderOrangeProgram = glCreateProgram();
    glAttachShader(shaderOrangeProgram, vertexShader);
    glAttachShader(shaderOrangeProgram, fragmentOrangeShader);
    glLinkProgram(shaderOrangeProgram);
    // check for linking errors
    glGetProgramiv(shaderOrangeProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderOrangeProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentOrangeShader);
    
    // link Yellow shaders
    int shaderYellowProgram = glCreateProgram();
    glAttachShader(shaderYellowProgram, vertexShader);
    glAttachShader(shaderYellowProgram, fragmentYellowShader);
    glLinkProgram(shaderYellowProgram);
    // check for linking errors
    glGetProgramiv(shaderYellowProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderYellowProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentYellowShader);
#endif
    
#if UNIFORM_DRAW
    // link uniform shaders
    int shaderUniformProgram = glCreateProgram();
    glAttachShader(shaderUniformProgram, vertexShader);
    glAttachShader(shaderUniformProgram, fragmentUniformShader);
    glLinkProgram(shaderUniformProgram);
    // check for linking errors
    glGetProgramiv(shaderUniformProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderUniformProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentUniformShader);
#endif
    
#if BLEND_DRAW
    // link Blend shaders
    int shaderBLendProgram = glCreateProgram();
    glAttachShader(shaderBLendProgram, vertexBlendShader);
    glAttachShader(shaderBLendProgram, fragmentBlendShader);
    glLinkProgram(shaderBLendProgram);
    // check for linking errors
    glGetProgramiv(shaderBLendProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderBLendProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexBlendShader);
    glDeleteShader(fragmentBlendShader);
#endif
    
#if OFFSET_DRAW
    // link Blend shaders
    int shaderOffsetProgram = glCreateProgram();
    glAttachShader(shaderOffsetProgram, vertexOffsetShader);
    glAttachShader(shaderOffsetProgram, fragmentOffsetShader);
    glLinkProgram(shaderOffsetProgram);
    // check for linking errors
    glGetProgramiv(shaderOffsetProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderOffsetProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexOffsetShader);
    glDeleteShader(fragmentOffsetShader);
#endif
    
#if BLEND_DRAW_ENCAPSOLUTION
    //note:absolute path is very important
    Shader blend_draw_Shader(SHADER_ROOT_DIR"shader.vs",SHADER_ROOT_DIR"shader.fs");
#endif
    
#if TEXTURE_BOX
    Shader texture_box_Shader(SHADER_ROOT_DIR"4.1.texture.vs", SHADER_ROOT_DIR"4.1.texture.fs");
#endif
    
#if TEXTURE_BOX_AND_WINDOW
    Shader texture_box_and_window_Shader(SHADER_ROOT_DIR"4.2.texture.vs", SHADER_ROOT_DIR"4.2.texture.fs");
#endif
    
#if TEXTURE_MATRIX_OP
    Shader texture_matrix_op_Shader(SHADER_ROOT_DIR"5.1.transform.vs", SHADER_ROOT_DIR"5.1.transform.fs");
#endif
    
#if COORDINATE_SYSTEM
    Shader texture_matrix_op_Shader(SHADER_ROOT_DIR"6.1.coordinate_systems.vs", SHADER_ROOT_DIR"6.1.coordinate_systems.fs");
#endif
    
#if COORDINATE_SYSTEM_CUBE_ROTATE
    Shader texture_matrix_op_Shader(SHADER_ROOT_DIR"6.2.coordinate_systems.vs", SHADER_ROOT_DIR"6.2.coordinate_systems.fs");
#endif
    
#if COORDINATE_SYSTEM_MULTI_CUBE_ROTATE
    Shader texture_matrix_op_Shader(SHADER_ROOT_DIR"6.3.coordinate_systems.vs", SHADER_ROOT_DIR"6.3.coordinate_systems.fs");
#endif

#if CAMERA_VIEW | CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
    Shader texture_matrix_op_Shader(SHADER_ROOT_DIR"7.1.camera.vs", SHADER_ROOT_DIR"7.1.camera.fs");
#endif
    
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // add a new set of vertices to form a second triangle (a total of 6 vertices); the vertex attribute configuration remains the same (still one 3-float position vector per vertex)
#if 0
    float vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f,  // top
        // second triangle
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top
    };
#endif
    
#if DOUBLE_TRI_DRAW
    float first_vertices[] = {
        // first triangle
        -0.9f, -0.5f, 0.0f,  // left
        -0.0f, -0.5f, 0.0f,  // right
        -0.45f, 0.5f, 0.0f  // top
    };
    
    float second_vertices[] = {
        // second triangle
        0.0f, -0.5f, 0.0f,  // left
        0.9f, -0.5f, 0.0f,  // right
        0.45f, 0.5f, 0.0f   // top
    };
#endif

#if UNIFORM_DRAW
#if 1
    float uniform_vertices[] = {
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f   // top
    };
#else
    float uniform_vertices[] = { //handstand just minus y
        0.5f, 0.5f, 0.0f,  // bottom right
       -0.5f, 0.5f, 0.0f,  // bottom left
        0.0f, -0.5f, 0.0f   // top
    };
#endif
#endif

#if OFFSET_DRAW
    float offset_vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
    };
#endif

#if BLEND_DRAW | BLEND_DRAW_ENCAPSOLUTION
    float blend_vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
       -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
    };
#endif

#if TEXTURE_BOX | TEXTURE_BOX_AND_WINDOW
#if 1
    float texture_box_vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
#else
    float texture_box_vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.6f, 0.75f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.75f, 0.25f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.25f, 0.25f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.4f, 0.75f  // top left
    };
#endif
    
    unsigned int texture_box_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
#endif

#if TEXTURE_MATRIX_OP | COORDINATE_SYSTEM
    float texture_matrix_op_vertices[] = {
        // positions          // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int texture_matrix_op_indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
#endif
    
#if COORDINATE_SYSTEM_CUBE_ROTATE | COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | \
    CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
    float texture_matrix_op_vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
#endif
    
#if COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | CAMERA_VIEW_INPUT | \
    CAMERA_VIEW_MOUSE_AND_ZOOM
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
#endif
    
#if DOUBLE_TRI_DRAW
    unsigned int VBO[2], VAO[2];
    glGenVertexArrays(2,VAO);
    glGenBuffers(2,VBO);
    
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(first_vertices), first_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(second_vertices), second_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif
    
#if UNIFORM_DRAW
    //uniform vertices
    unsigned int VBO_uniform, VAO_uniform;
    glGenVertexArrays(1,&VAO_uniform);
    glGenBuffers(1,&VBO_uniform);
    
    glBindVertexArray(VAO_uniform);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_uniform);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uniform_vertices), uniform_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif
    
#if OFFSET_DRAW
    //uniform vertices
    unsigned int VBO_offset, VAO_offset;
    glGenVertexArrays(1,&VAO_offset);
    glGenBuffers(1,&VBO_offset);
    
    glBindVertexArray(VAO_offset);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_offset);
    glBufferData(GL_ARRAY_BUFFER, sizeof(offset_vertices), offset_vertices, GL_STATIC_DRAW);
    
    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif
    
#if BLEND_DRAW | BLEND_DRAW_ENCAPSOLUTION
    unsigned int VBO_blend, VAO_blend;
    glGenVertexArrays(1,&VAO_blend);
    glGenBuffers(1,&VBO_blend);
    
    glBindVertexArray(VAO_blend);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_blend);
    glBufferData(GL_ARRAY_BUFFER, sizeof(blend_vertices), blend_vertices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif
    
#if TEXTURE_BOX | TEXTURE_BOX_AND_WINDOW
    unsigned int texture_box_VBO, texture_box_VAO, texture_box_EBO;
    glGenVertexArrays(1, &texture_box_VAO);
    glGenBuffers(1, &texture_box_VBO);
    glGenBuffers(1, &texture_box_EBO);
    
    glBindVertexArray(texture_box_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, texture_box_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_box_vertices), texture_box_vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_box_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texture_box_indices), texture_box_indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
#endif
    
#if TEXTURE_MATRIX_OP | COORDINATE_SYSTEM | COORDINATE_SYSTEM_CUBE_ROTATE | \
    COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | CAMERA_VIEW_INPUT | \
    CAMERA_VIEW_MOUSE_AND_ZOOM
    unsigned int texture_matrix_op_VBO, texture_matrix_op_VAO;
    glGenVertexArrays(1, &texture_matrix_op_VAO);
    glGenBuffers(1, &texture_matrix_op_VBO);

#if !COORDINATE_SYSTEM_CUBE_ROTATE & !COORDINATE_SYSTEM_MULTI_CUBE_ROTATE & !CAMERA_VIEW & \
    !CAMERA_VIEW_INPUT & !CAMERA_VIEW_MOUSE_AND_ZOOM
    unsigned int texture_matrix_op_EBO;
    glGenBuffers(1, &texture_matrix_op_EBO);
#endif
    
    glBindVertexArray(texture_matrix_op_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, texture_matrix_op_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_matrix_op_vertices), texture_matrix_op_vertices, GL_STATIC_DRAW);

#if !COORDINATE_SYSTEM_CUBE_ROTATE & !COORDINATE_SYSTEM_MULTI_CUBE_ROTATE & !CAMERA_VIEW & \
    !CAMERA_VIEW_INPUT & !CAMERA_VIEW_MOUSE_AND_ZOOM
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texture_matrix_op_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texture_matrix_op_indices), texture_matrix_op_indices, GL_STATIC_DRAW);
#endif
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
#endif
    
    //load and create a texture box
#if TEXTURE_BOX | TEXTURE_BOX_AND_WINDOW | TEXTURE_MATRIX_OP | COORDINATE_SYSTEM | \
    COORDINATE_SYSTEM_CUBE_ROTATE | COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | \
    CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
    unsigned int texture_box;
    glGenTextures(1, &texture_box);
    glBindTexture(GL_TEXTURE_2D, texture_box); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int texture_box_width, texture_box_height, texture_box_nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *texture_box_data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &texture_box_width, &texture_box_height, &texture_box_nrChannels, 0);
    if (texture_box_data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_box_width, texture_box_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_box_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(texture_box_data);
#endif
    //load and create a texture face
#if TEXTURE_BOX_AND_WINDOW | TEXTURE_MATRIX_OP | COORDINATE_SYSTEM | \
    COORDINATE_SYSTEM_CUBE_ROTATE | COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | \
    CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
    unsigned int texture_face;
    glGenTextures(1, &texture_face);
    glBindTexture(GL_TEXTURE_2D, texture_face);
    // set the texture wrapping parameters,设置纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);    // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters，设置，放大/缩小 采用的环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int texture_face_width, texture_face_height, texture_face_nrChannels;
    //stbi_set_flip_vertically_on_load(true);
    unsigned char *texture_face_data = stbi_load(FileSystem::getPath("resources/textures/toy_box_disp.png").c_str(), &texture_face_width, &texture_face_height, &texture_face_nrChannels, 0);
    if (texture_face_data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_face_width, texture_face_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_face_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture face" << std::endl;
    }
    stbi_image_free(texture_face_data);
#endif

#if TEXTURE_BOX_AND_WINDOW
    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    texture_box_and_window_Shader.use(); // don't forget to activate/use the shader before setting uniforms!
    //在激活着色器前先设置uniform！或者使用着色器类设置
    // either set it manually like so:
    //glUniform1i(glGetUniformLocation(texture_box_and_window_Shader.ID, "texture_box"), 0);
    //glUniform1i(glGetUniformLocation(texture_box_and_window_Shader.ID, "texture_face"), 1);
    // or set it via the texture class
    texture_box_and_window_Shader.setInt("texture_box", 0);
    texture_box_and_window_Shader.setInt("texture_face", 1);
#endif
    
#if TEXTURE_MATRIX_OP | COORDINATE_SYSTEM | COORDINATE_SYSTEM_CUBE_ROTATE | \
    COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | CAMERA_VIEW_INPUT | \
    CAMERA_VIEW_MOUSE_AND_ZOOM
    texture_matrix_op_Shader.use();
    texture_matrix_op_Shader.setInt("texture_box", 0);
    texture_matrix_op_Shader.setInt("texture_face", 1);
#endif

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //-------------------------------------------------------------------------------------------
    // render loop

#if COORDINATE_SYSTEM_CUBE_ROTATE | COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | \
    CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM //Z-buffer use
    glEnable(GL_DEPTH_TEST);
#endif

#if CAMERA_VIEW | CAMERA_VIEW_INPUT
    // pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    texture_matrix_op_Shader.setMat4("projection", projection);
#endif
    
    while (!glfwWindowShouldClose(window))
    {
#if CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
#endif
        // input process
        processInput(window);
        
        // render process
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        
#if COORDINATE_SYSTEM_CUBE_ROTATE | COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | \
    CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
        //z-buffer use
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!
#else
        glClear(GL_COLOR_BUFFER_BIT);
#endif

#if DOUBLE_TRI_DRAW
        // draw our first triangle
        glUseProgram(shaderOrangeProgram);
        glBindVertexArray(VAO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        //draw our second triangle
        glUseProgram(shaderYellowProgram);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
#endif

#if UNIFORM_DRAW
        //draw uniform triangle
        glUseProgram(shaderUniformProgram);
        // update shader uniform
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderUniformProgram, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        glBindVertexArray(VAO_uniform);
        glDrawArrays(GL_TRIANGLES, 0, 3);
#endif
        
#if BLEND_DRAW
        //draw blend triangle
        glUseProgram(shaderBLendProgram);
        glBindVertexArray(VAO_blend);
        glDrawArrays(GL_TRIANGLES, 0, 3);
#endif

#if OFFSET_DRAW
        //draw offset triangle
        glUseProgram(shaderOffsetProgram);
        //设置根据时间变换的x，y偏移值，最终效果为圆周运动
        float timeValue = glfwGetTime();
        float offsetx = (sin(timeValue) / 2) + 0.5;
        float offsety = (cos(timeValue) / 2) + 0.5;
        int vertexColorLocation = glGetUniformLocation(shaderOffsetProgram, "offset");
        glUniform4f(vertexColorLocation, offsetx, offsety, 0.0f, 1.0f);
        glBindVertexArray(VAO_offset);
        glDrawArrays(GL_TRIANGLES, 0, 3);
#endif
        
#if BLEND_DRAW_ENCAPSOLUTION
        //draw blend triangle form shader.vs & shader.fs
        blend_draw_Shader.use();
        float timeValue1 = glfwGetTime();
        float greenValue1 = sin(timeValue1) / 2.0f + 0.5f;
        blend_draw_Shader.setFloat("xOffset",greenValue1);
        glBindVertexArray(VAO_blend);
        glDrawArrays(GL_TRIANGLES, 0, 3);
#endif
        // glBindVertexArray(0); // no need to unbind it every time

#if TEXTURE_BOX
        // bind Texture
        //glActiveTexture(GL_TEXTURE0);//default,so hide
        glBindTexture(GL_TEXTURE_2D, texture_box);

        // render container
        texture_box_Shader.use();
        glBindVertexArray(texture_box_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#endif
        
#if TEXTURE_BOX_AND_WINDOW | TEXTURE_MATRIX_OP | COORDINATE_SYSTEM | \
        COORDINATE_SYSTEM_CUBE_ROTATE | COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | \
        CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_box);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_face);
#endif
        
#if TEXTURE_BOX_AND_WINDOW
        // render container
        texture_box_and_window_Shader.use();
        glBindVertexArray(texture_box_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#endif

#if TEXTURE_MATRIX_OP
        // create transformations
        glm::mat4 transform;
        //transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        // get matrix's uniform location and set matrix
        texture_matrix_op_Shader.use();
        unsigned int transformLoc = glGetUniformLocation(texture_matrix_op_Shader.ID, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
        
        // render container
        glBindVertexArray(texture_matrix_op_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#if 1 //transform matrix
        transform = glm::mat4(); // reset it to an identity matrix
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.5f, 0.0f));
        float scaleAmount = sin(glfwGetTime());
        transform = glm::scale(transform, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, &transform[0][0]); // this time take the matrix value array's first element as its memory pointer value
        
        // now with the uniform matrix being replaced with new transformations, draw it again.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#endif
#endif
        
#if COORDINATE_SYSTEM
        // create transformations
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        model = glm::rotate(model, glm::radians(55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(texture_matrix_op_Shader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(texture_matrix_op_Shader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        texture_matrix_op_Shader.setMat4("projection", projection);
        glBindVertexArray(texture_matrix_op_VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#endif
        
#if COORDINATE_SYSTEM_CUBE_ROTATE
        // activate shader
        texture_matrix_op_Shader.use();
        
        // create transformations
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(texture_matrix_op_Shader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(texture_matrix_op_Shader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        texture_matrix_op_Shader.setMat4("projection", projection);
        
        // render box
        glBindVertexArray(texture_matrix_op_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
#endif
        
#if COORDINATE_SYSTEM_MULTI_CUBE_ROTATE
        texture_matrix_op_Shader.use();
        
        // create transformations
        glm::mat4 view;
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // pass transformation matrices to the shader
        texture_matrix_op_Shader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        texture_matrix_op_Shader.setMat4("view", view);
        
        // render boxes
        glBindVertexArray(texture_matrix_op_VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            texture_matrix_op_Shader.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
#endif
        
#if CAMERA_VIEW
        // camera/view transformation
        glm::mat4 view;
        float radius = 10.0f;
        float camX   = sin(glfwGetTime()) * radius;
        float camZ   = cos(glfwGetTime()) * radius;
        view = glm::lookAt(glm::vec3(camX, 0.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
#endif
        
#if CAMERA_VIEW_INPUT
        // activate shader
        texture_matrix_op_Shader.use();
        // camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
#endif
        
#if CAMERA_VIEW_MOUSE_AND_ZOOM
        texture_matrix_op_Shader.use();
#if USE_CAMERA_CLASS
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
#else
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
#endif
        texture_matrix_op_Shader.setMat4("projection", projection);
        
        // camera/view transformation
#if USE_CAMERA_CLASS
        glm::mat4 view = camera.GetViewMatrix();
#else
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
#endif
        
#endif
        
#if CAMERA_VIEW | CAMERA_VIEW_INPUT | CAMERA_VIEW_MOUSE_AND_ZOOM
        texture_matrix_op_Shader.setMat4("view", view);
        // render boxes
        glBindVertexArray(texture_matrix_op_VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            texture_matrix_op_Shader.setMat4("model", model);
            
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
#endif
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
#if DOUBLE_TRI_DRAW
    glDeleteVertexArrays(2,VAO);
    glDeleteBuffers(2,VBO);
#endif
    
#if UNIFORM_DRAW
    glDeleteVertexArrays(1,&VAO_uniform);
    glDeleteBuffers(1,&VBO_uniform);
#endif
    
#if BLEND_DRAW | BLEND_DRAW_ENCAPSOLUTION
    glDeleteVertexArrays(1,&VAO_blend);
    glDeleteBuffers(1,&VBO_blend);
#endif

#if OFFSET_FRAW
    glDeleteVertexArrays(1,&VAO_offset);
    glDeleteBuffers(1,&VBO_offset);
#endif
    
#if TEXTURE_BOX | TEXTURE_BOX_AND_WINDOW
    glDeleteVertexArrays(1, &texture_box_VAO);
    glDeleteBuffers(1, &texture_box_VBO);
    glDeleteBuffers(1, &texture_box_EBO);
#endif
    
#if TEXTURE_MATRIX_OP | COORDINATE_SYSTEM | COORDINATE_SYSTEM_CUBE_ROTATE | \
    COORDINATE_SYSTEM_MULTI_CUBE_ROTATE | CAMERA_VIEW | CAMERA_VIEW_INPUT | \
    CAMERA_VIEW_MOUSE_AND_ZOOM
    glDeleteVertexArrays(1, &texture_matrix_op_VAO);
    glDeleteBuffers(1, &texture_matrix_op_VBO);
#if !COORDINATE_SYSTEM_CUBE_ROTATE & !COORDINATE_SYSTEM_MULTI_CUBE_ROTATE & !CAMERA_VIEW & \
    !CAMERA_VIEW_INPUT & !CAMERA_VIEW_MOUSE_AND_ZOOM
    glDeleteBuffers(1, &texture_matrix_op_EBO);
#endif
#endif
    
    glfwTerminate(); // glfw: terminate, clearing all previously allocated GLFW resources.
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
#if CAMERA_VIEW_INPUT
#if USE_CAMERA_CLASS
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
#else
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
#endif
#endif
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
#if CAMERA_VIEW_MOUSE_AND_ZOOM
// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    
#if USE_CAMERA_CLASS
    camera.ProcessMouseMovement(xoffset, yoffset);
#else
    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)); //* cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
#endif
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
#if USE_CAMERA_CLASS
    camera.ProcessMouseScroll(yoffset);
#else
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
#endif
}
#endif
