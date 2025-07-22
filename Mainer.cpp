#include <MeGlWindow.h>

int main(int argc, char* argv[]) {
    MeGlWindow window(800, 600, "OpenGL By Jamie King");

    if(!window.Init()) { return -1; }

    window.MainLoop();

    return 0;
}
