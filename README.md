# vulkan-cmake-template
cmake template project for vulkan C++ with GLFW, GLM, STB, TINYOBJLOADER and ImGUI

### Structure of the project :

|
+-- assets (you need to create it)
+-- cmake
| +-- LinkGLFW.cmake
| +-- LinkGLM.cmake
| +-- LinkImGUI.cmake
| +-- LinkSTB.cmake
| +-- LinkTINYOBJLOADER.cmake
|
+-- include
| +-- vulkanbase.h
|
+-- src
| +-- main.cpp
|
+-- CMakeLists.txt

Each cmake sub-macro will fetch the corresponding git dependency and link it to the project.

 
### How to start

#### Linux
mkdir assets
mkdir build
cd build
cmake ..

#### Windows Visual Studio
mkdir assets
open project with VS
