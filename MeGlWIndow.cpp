#include "MeGlWindow.h"

MeGlWindow::MeGlWindow(int width, int height, const std::string& title) 
    : width_(width), height_(height), title_(title), window_(nullptr)
{
}

MeGlWindow::~MeGlWindow() 
{
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

extern const char* vertexShaderCode;
extern const char* fragmentShaderCode;

void sendDataToOpenGL()
{
     GLfloat verts[] = {
        0.0f, 0.0f, //position attrib
            1.0f, 0.0f, 0.0f, // color attrib
        1.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f,
            1.0f, 0.0f, 0.0f,

        // 0.0f, 0.0f, as we removed this vertex, we will have two triangles(see indices[]) as: 0,1,2 & 0,3,4 (recomm. to respect winding order, here anticlock)
        -1.0f, -1.0f,
            1.0f, 0.0f, 0.0f,
        1.0f, -1.0f,
            1.0f, 0.0f, 0.0f,
    };

    // Buffer Objects
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW); // copies the array data to the buffer, with ID vertexBufferId

    GLuint firstAttrib = 0; GLint noOfElementsInFirstAttrib = 2; GLsizei stride = 5 * sizeof(GLfloat); // stride is the distance from one set of attribs to another set of attribs, ie 'i'th Vertex data to 'i+1'th Vertex data
    glEnableVertexAttribArray(firstAttrib);
    // now describe the array, like how to split, how to index the attribs, etc.
    glVertexAttribPointer(firstAttrib, noOfElementsInFirstAttrib, GL_FLOAT, GL_FALSE, stride, 0);
    //                                                          type of elements, should gl normalize the data, stride, ptr

    // now for color attrib(second attrib) of each vertex
    GLuint secondAttrib = 1; GLint noOfElementsInSecondAttrib = 3; GLsizei strideWrtSecondAttrib = 5 * sizeof(GLfloat); GLsizei sizeBeforeToReachTheFirst_SecondAttrib = 2 * sizeof(GLfloat);
    glEnableVertexAttribArray(secondAttrib);
    glVertexAttribPointer(secondAttrib, noOfElementsInSecondAttrib, GL_FLOAT, GL_FALSE, strideWrtSecondAttrib, (GLsizei*)(sizeBeforeToReachTheFirst_SecondAttrib)); // since opengl is old, cast it to any type of ptr
    
    // indices (or index array buffers or element array buffers) are used to get rid off redundant vertices
    GLushort indices[] = {0,1,2,  0,3,4};
    GLuint indexBufferId;
    glGenBuffers(1, &indexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void installShaders()
{
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER); // create VS object
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER); // create FS object

    const char* adapter[1]; // just an extra ptr that points to the shader code ptr, bcoz yeah, this C API wants **string (two ptrs in chain), so this extra work

    adapter[0] = vertexShaderCode;
    glShaderSource(vertexShaderId, 1, adapter, NULL);
    adapter[0] = fragmentShaderCode;
    glShaderSource(fragmentShaderId, 1, adapter, NULL);

    glCompileShader(vertexShaderId); // these two output files like objs, just like cpp compiler does from .cpp to .obj
    glCompileShader(fragmentShaderId);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId); // similar to taking objs and linking thme together to create a simgle program
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);

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

    sendDataToOpenGL();
    installShaders();

    return true;
}

void MeGlWindow::MainLoop()
{
    GLint firstTriangle = 0; // traingel in 0th idx
    GLsizei verticesToRender = 3 + 3;

    while(!glfwWindowShouldClose(window_)) {
        int w, h;
        glfwGetFramebufferSize(window_, &w, &h);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Optional background color
        glClear(GL_COLOR_BUFFER_BIT); // GLAD fn
        glViewport(0,0, w, h); // adjust drawings on window resize

        // glDrawArrays(GL_TRIANGLES, firstTriangle, verticesToRender);
        glDrawElements(GL_TRIANGLES, verticesToRender, GL_UNSIGNED_SHORT, 0); //  the last arg is not truly a ptr, it takes the offsets of index buffer, now nothing, so 0 or NULL
        //                      6 to render, from 5 available


        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}