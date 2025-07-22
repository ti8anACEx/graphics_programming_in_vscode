# 🧊 GLFW Setup on Windows (MinGW + vcpkg) – Jit Deb Nath's Notes

This project demonstrates a clean, local C++ + OpenGL + GLFW development setup on Windows using:
- **MinGW-w64 (G++)**
- **GLFW (from vcpkg)**
- **Makefile for builds**

This README covers:
1. ✅ The successful final setup
2. ❌ The wrong paths taken (like using MSVC binaries with MinGW)
3. 🔧 How the Makefile works
4. 📦 Where I copied files from and why

---

## 📌 Objective

I wanted to:
- Use **MinGW** with `g++` on Windows.
- Build a local OpenGL + GLFW-based project.
- Avoid relying on MSVC.
- Make the setup portable and fully offline if needed (by copying the required libs/headers locally).

---

## ✅ Final Working Setup

### 🧰 Toolchain

| Component      | Info                                                  |
|----------------|--------------------------------------------------------|
| Compiler       | `g++ (x86_64-posix-seh, Built by MinGW-W64) 8.1.0`     |
| Build Tool     | `make` (used via Git Bash / MSYS2)                     |
| GLFW Install   | via `vcpkg install glfw3:x64-mingw-static`            |
| Platform       | Windows 10/11                                          |

---

## ⚙️ Installing GLFW with vcpkg (for MinGW)

```bash
# Clone vcpkg if you haven't:
git clone https://github.com/microsoft/vcpkg
cd vcpkg
./bootstrap-vcpkg.bat

# Install GLFW for MinGW-w64 (static)
./vcpkg install glfw3:x64-mingw-static

Note: This gives you:

    Static .a libraries for MinGW (libglfw3.a, etc.)

    Headers like glfw3.h, glfw3native.h


Copying Files Locally (for portability and also, i wanted to make my project fully local, so that it can be zipped and extracted directly for ready to use)
After installation, I copied the following to my project root folder:

project_root/
│
├── include/                  ← Headers
│   └── GLFW/                 ← From: vcpkg/installed/x64-mingw-static/include/GLFW
│       ├── glfw3.h
│       └── glfw3native.h
│
├── lib/                      ← Static libraries
│   ├── libglfw3.a            ← From: vcpkg/installed/x64-mingw-static/lib/
│   ├── libglfw3dll.a         (Not required in this vcpkg installation process)
│   └── libglfw3main.a        (Not required in this vcpkg installation process)
├── main.cpp                    
├── Makefile                    

🧠 Mistakes & Errors I Made
❌ 1. Tried to use GLFW Precompiled Binaries from the official website

    Problem: These are MSVC-only .lib files.

    Why It Failed:

        MinGW’s g++ cannot link .lib files meant for MSVC.

        Linking errors like undefined reference to glfwInit occurred.

✅ Solution: Always use MinGW-built .a files (like from vcpkg install glfw3:x64-mingw-static) when using g++.


------ If I am copying files from vcpkg/installed/glfw3/ the Makefile looks like this::

CXX = g++
CXXFLAGS = -Ilib/glfw/include
LDFLAGS = lib/glfw/lib/libglfw3.a -lopengl32 -lgdi32 -lwinmm

TARGET = main.exe
SRC = main.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	del $(TARGET)


------ Else If I am using files directly from vcpkg/installed/glfw3/ the Makefile looks like this (also no need to have the lib folder locally in the project folder)::

CXX = g++
CXXFLAGS = -IC:/Users/ASUS/vcpkg/installed/x64-mingw-static/include
LDFLAGS = -LC:/Users/ASUS/vcpkg/installed/x64-mingw-static/lib -lglfw3 -lopengl32 -lgdi32 -lwinmm


TARGET = main.exe
SRC = main.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	del $(TARGET)


----------------------------------------------------------------------------------------------------------

----I also askeed ChatGPT about this:

ok, before moving to next, the glfw website says 
"Windows pre-compiled binaries

These packages contain the GLFW header files, documentation and release mode static libraries, DLLs and import libraries for Visual C++ 2013-2022 and MinGW-w64." (focus on MinGW)

and also provides lib-mingw-w64 folder with the static and dynamic files. But why that did not work with my MinGW GCC g++ compiler when i copied the include and lib-mingw-w64 folder containing the .a file? Instead i had to separately install glfw3:x64-mingw-static with vcpkg and then copy the static lib from there, containing the .a file and the include dir?


----ChatGPT answered:

## 🧩 Why the Official GLFW Binaries Didn't Work

Although the [GLFW website](https://www.glfw.org/download.html) claims to support **MinGW-w64**, the precompiled binaries provided often assume:

* A specific **version** of MinGW-w64 (often built using older GCC toolchains)
* Specific **linking settings** (e.g., static vs. dynamic runtime)
* Sometimes **MSVC-style naming or linking formats**

In your case:

* You used `g++.exe (x86_64-posix-seh-rev0, Built by MinGW-W64 project) 8.1.0`
* You copied the official `include/` and `lib-mingw-w64/libglfw3dll.a`, but linking failed.

### 🚫 Why It Failed:

* The `.a` file from the official zip was either:

  * Not built with `posix-seh` threading model.
  * Incompatible with your MinGW’s runtime expectations.
  * Dependent on a different C runtime or toolchain version.

### ✅ Why vcpkg Worked:

* `vcpkg` compiled `glfw3:x64-mingw-static` using your *actual compiler* and toolchain.
* This ensured:

  * Proper binary compatibility
  * Matching C++ runtime
  * Seamless integration with your project

### 🔁 Final Insight:

Even if binaries are labeled “MinGW-w64,” they may not work with **your** specific MinGW-w64 build unless the compiler, runtime, and flags match exactly. That’s why **building GLFW from source or using vcpkg** is the safest route for MinGW projects.


------------------------------------------ NOW I INSTALLED GLAD (The OpenGL Function Ptr Loader) ------------------------------------------
Step 1: Visit GLAD Web Generator
Go to: https://gen.glad.sh/
    Language: C/C++
    Specification: OpenGL
    API: OpenGL (select version like 3.3, 4.1, or whatever you're targeting)
    Profile: Core (usually)
    Generate a loader: Yes (checked)
    Options: Leave default
    Click "Generate" and download the ZIP.

Step 2: Extract the following into your project directory
Inside the ZIP:
    Copy glad.c into your src/ or project root
    Copy include/glad → into your include/ folder (or lib/include)
    Copy include/KHR → alongside that

The folder structure looks like:

project_root/
│
├── include/
│   ├── GLFW/                  ← From vcpkg (already added)
│   ├── glad/                  ← NEW: from GLAD zip → include/glad/
│   └── KHR/                   ← NEW: from GLAD zip → include/KHR/
│
├── lib/
│   └── libglfw3.a             ← From vcpkg
│
├── src/
│   └── glad.c                 ← Moved here (for organization)
│
├── main.cpp
├── Makefile


and the new Makefile is

CXX = g++
CXXFLAGS = -Iinclude -Isrc
LDFLAGS = lib/libglfw3.a -lopengl32 -lgdi32 -lwinmm

TARGET = main.exe
SRC = main.cpp src/glad.c

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	del $(TARGET)


Now, run mingw-make or make to build and then .\main.exe to run