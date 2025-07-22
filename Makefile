# Using Windows Powershell as my terminal

CXX = g++
CXXFLAGS = -Iinclude -I.
LDFLAGS = lib/libglfw3.a -lopengl32 -lgdi32 -lwinmm

TARGET = Mainer.exe
SRC = Mainer.cpp MeGlWindow.cpp MeShaderCode.cpp src/glad.c

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)
	.\$(TARGET)
	
run:
	.\$(TARGET)

clean:
	del $(TARGET)
