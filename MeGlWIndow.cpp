#include "MeGlWindow.h"
#include<fstream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include<glm/gtx/transform.hpp>
#include<Primitives/Vertex.h>
#include<Primitives/ShapeGenerator.h>
#include<imgui/imgui.h>
#include<imgui/backends/imgui_impl_glfw.h>
#include<imgui/backends/imgui_impl_opengl3.h>

// extern const char* vertexShaderCode;
// extern const char* fragmentShaderCode;

const float X_DELTA = 0.1;
unsigned int numOfTriangles = 0;
const unsigned int NUM_VERTICES_PER_TRIANGLE = 3;
const unsigned int NUM_FLOATS_PER_VERTEX = 6;
const unsigned int TRIANGLE_SIZE_IN_BYTES = NUM_VERTICES_PER_TRIANGLE * NUM_FLOATS_PER_VERTEX * sizeof(GLfloat);
const unsigned int MAX_TRAINGLES = 20;
GLuint programId;
GLuint numIndices;

MeGlWindow::MeGlWindow(int width, int height, const std::string& title) 
    : width_(width), height_(height), title_(title), window_(nullptr)
{
}

MeGlWindow::~MeGlWindow() 
{
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glUseProgram(0); // dont use any shader program or stop using esisting programs
    glDeleteProgram(programId); // if a shader is in use, delete will not have any effect
    glfwTerminate();
}


void MeGlWindow::sendDataToOpenGL()
{
    ShapeData shape = ShapeGenerator::makeCube();

    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, shape.vertexBufferSize(), shape.vertices, GL_STATIC_DRAW); // copies the array data to the buffer, with ID vertexBufferId

    GLuint firstAttrib = 0; GLint noOfElementsInFirstAttrib = 3; GLsizei stride = 6 * sizeof(GLfloat); // stride is the distance from one set of attribs to another set of attribs, ie 'i'th Vertex data to 'i+1'th Vertex data
    glEnableVertexAttribArray(firstAttrib);
    // now describe the array, like how to split, how to index the attribs, etc.
    glVertexAttribPointer(firstAttrib, noOfElementsInFirstAttrib, GL_FLOAT, GL_FALSE, stride, 0);
    //                                                          type of elements, should gl normalize the data, stride, ptr

    // now for color attrib(second attrib) of each vertex
    GLuint secondAttrib = 1; GLint noOfElementsInSecondAttrib = 3; GLsizei strideWrtSecondAttrib = stride; GLsizei sizeBeforeToReachTheFirst_SecondAttrib = 3 * sizeof(GLfloat);
    glEnableVertexAttribArray(secondAttrib);
    // or we can do this to get a varying/uniform like behaviour: glVertexAttrib3f(secondAttrib, 0.0f, 1.0f, 1.0f); but comment the previous line.
    glVertexAttribPointer(secondAttrib, noOfElementsInSecondAttrib, GL_FLOAT, GL_FALSE, strideWrtSecondAttrib, (GLsizei*)(sizeBeforeToReachTheFirst_SecondAttrib)); // since opengl is old, cast it to any type of ptr

    GLuint indexBufferId;
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, shape.indexBufferSize(), shape.indices, GL_STATIC_DRAW);
    numIndices = shape.numIndices;
    shape.cleanup();

    GLuint transformationMatrixBufferId;
    glGenBuffers(1, &transformationMatrixBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, transformationMatrixBufferId);
    // TODO:                                        fix aspect ratio thing, if window size changes cube wont be projected properly
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), ((float)width_) / height_, 0.1f, 10.0f); // actually its -0.1 and -10.0 since camera looks at -Z of RH Coord Sys
    glm::mat4 fullTransforms[] = {
        // FOR FIRST CUBE, using glm/gtx library
        projectionMatrix * glm::translate(glm::vec3(-1.0f, 0.0f, -3.0f)) * glm::rotate(glm::radians(36.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        // SECOND CUBE 
        projectionMatrix * glm::translate(glm::vec3(1.0f, 0.0f, -3.75f)) * glm::rotate(glm::radians(126.0f), glm::vec3(0.0f, 1.0f, 0.0f))
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullTransforms), fullTransforms, GL_STATIC_DRAW);
    // although we wanna send 16 GL_FLOATS, the max we can send at once is 4 (also the attrib number increments after 4 gaps, so keep track of it)
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(GLfloat) * 0));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(GLfloat) * 4));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(GLfloat) * 8));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(GLfloat) * 12));

    glEnableVertexAttribArray(2); glEnableVertexAttribArray(3); glEnableVertexAttribArray(4); glEnableVertexAttribArray(5);
        // Do instancing stuff now
        GLuint attribNumber = 2, divisor = 1;
        glVertexAttribDivisor(attribNumber, divisor); // divisor divides the instance numbers to help out get a rate of applying the attribute (offsets attrib) to that instance. 'offests' therefore varies per instance now unlike 'vertexBuffer' which varies per vertex (default behavior)
        glVertexAttribDivisor(3, 1); glVertexAttribDivisor(4, 1); glVertexAttribDivisor(5, 1); 
        // then do glDrawElementsInstanced() in MainLoop
}

void sendAnotherTriangleToOpenGl() {
    if (numOfTriangles == MAX_TRAINGLES) {
        return;
    }
    
    const GLfloat THIS_TRI_X = -1 + numOfTriangles * X_DELTA;
    GLfloat thisTriangle[] = {
        THIS_TRI_X, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        THIS_TRI_X + X_DELTA, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        THIS_TRI_X, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
    };

    unsigned int offset = numOfTriangles * TRIANGLE_SIZE_IN_BYTES;
    glBufferSubData(GL_ARRAY_BUFFER, offset, TRIANGLE_SIZE_IN_BYTES, thisTriangle);
    numOfTriangles++;
}

void MeGlWindow::MainLoop() // equivalent to paintGL() func in Jamie King's playlist
{
    { // ImGUI Init
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui::StyleColorsDark();
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    GLint firstVertexIdx = 0; // trainge in 0th idx. OR firstVertex ie vertex in 0th idx
    GLsizei verticesToRender = 3 + 3;

    while(!glfwWindowShouldClose(window_)) {
        { // ImGUI Render Loop Init
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        glfwGetFramebufferSize(window_, &width_, &height_);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Optional background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // fn ptr loaded from GLAD
        glViewport(0,0, width_, height_); // adjust drawings on window resize


        // glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
        glDrawElementsInstanced(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0, 2);

        { // ImGUI UI
            ImGui::Begin("My Window");
            ImGui::Text("Hello from ImGui!");
            ImGui::End();
        }

        { // ImGUI Rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    { // ImGUI terminate
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
    }
    ImGui::DestroyContext();
}
bool checkStatus(GLuint objectId,
    PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
    PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
    GLenum statusType
)
{
    // Error report and logging
    GLint status;
    objectPropertyGetterFunc(objectId, statusType, &status); //'iv' means compileStatus can be GLint or a vector of ints, ie GLint arr[]
    if (status != GL_TRUE)
    {
        GLint infoLogLength; // required info log lenth, can be set specifically by us, say 100, so it will limit the info length to max 100 chars
        objectPropertyGetterFunc(objectId, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* buffer = new GLchar[infoLogLength];

        GLsizei bufferSizeAgain; // buffer size as again given or confirmed by OpenGL. LOL.
        getInfoLogFunc(objectId, infoLogLength, &bufferSizeAgain, buffer);
        std::cout << buffer << std::endl;

        delete [] buffer;
        return false;
    }
    return true;
}



bool checkShaderStatus(GLint shaderId) 
{
    return checkStatus(shaderId, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool checkProgramStatus(GLint programId) 
{
    return checkStatus(programId, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

std::string readShaderCode(const char* fileName) 
{
    std::ifstream meInput(fileName);
    if (!meInput.good())
    {
        std::cout << "File failed to load..." << fileName << std::endl;
        exit(1);
    }
    return std::string(
        std::istreambuf_iterator<char>(meInput),
        std::istreambuf_iterator<char>()
    );
}

void installShaders()
{
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); // create VS object
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER); // create FS object

    const GLchar* adapter[1]; // just an extra ptr that points to the shader code ptr, bcoz yeah, this C API wants **string (two ptrs in chain), so this extra work
    std::string temp = readShaderCode("VertexShaderCode.glsl"); // imp: or else destructor gets called before c_str()
    adapter[0] = temp.c_str();
    glShaderSource(vertexShaderId, 1, adapter, NULL);
    
    temp = readShaderCode("FragmentShaderCode.glsl");
    adapter[0] = temp.c_str();
    glShaderSource(fragmentShaderId, 1, adapter, NULL);

    glCompileShader(vertexShaderId); // these two output files like objs, just like cpp compiler does from .cpp to .obj
    glCompileShader(fragmentShaderId);

    if (!checkShaderStatus(vertexShaderId) || !checkShaderStatus(fragmentShaderId)) {
        std::cout << "Stopped because compilation failed" << std::endl;
        return;
    }
    
    programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId); // similar to taking objs and linking thme together to create a simgle program
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

    if (!checkProgramStatus(programId)) {
        std::cout << "Stopped because linking failed" << std::endl;
        return;
    }

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);

    glUseProgram(programId);
}

bool MeGlWindow::Init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }
    
    window_ = glfwCreateWindow(width_, height_, title_.c_str(), NULL, NULL);
    
    if(!window_) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window_);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    sendDataToOpenGL();
    installShaders();

    return true;
}
