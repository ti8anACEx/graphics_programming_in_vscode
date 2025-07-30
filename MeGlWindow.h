#ifndef ME_GL_WINDOW
#define ME_GL_WINDOW

#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>

class MeGlWindow
{
public:
    MeGlWindow(int width, int height, const std::string& title);  
    ~MeGlWindow();

    bool Init();
    void sendDataToOpenGL();
    void MainLoop();

private:
    int width_;
    int height_;
    std::string title_;
    GLFWwindow* window_;
};

#endif