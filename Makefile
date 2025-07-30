# Using Windows Powershell as my terminal

CXX = g++
CXXFLAGS = -Iinclude -I. -Iinclude/imgui -Iinclude/imgui/backends
LDFLAGS = lib/libglfw3.a -lopengl32 -lgdi32 -lwinmm

TARGET = Mainer.exe
SRC = Mainer.cpp MeGlWindow.cpp Primitives/ShapeGenerator.cpp src/glad.c \
	src/imgui/imgui.cpp \
	src/imgui/imgui_draw.cpp \
	src/imgui/imgui_widgets.cpp \
	src/imgui/imgui_tables.cpp \
	src/imgui/backends/imgui_impl_glfw.cpp \
	src/imgui/backends/imgui_impl_opengl3.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	.\$(TARGET)
	
run:
	.\$(TARGET)

clean:
	del $(TARGET)
