#pragma once

// libs
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class KeyboardController
{
public:
    
	struct KeyMappings
	{
        int moveLeft = GLFW_KEY_A;
        int moveRight = GLFW_KEY_D;
        int moveForward = GLFW_KEY_W;
        int moveBackward = GLFW_KEY_S;
        int moveUp = GLFW_KEY_SPACE;
        int moveDown = GLFW_KEY_LEFT_SHIFT;
        int lookLeft = GLFW_KEY_LEFT;
        int lookRight = GLFW_KEY_RIGHT;
        int lookUp = GLFW_KEY_UP;
        int lookDown = GLFW_KEY_DOWN;
	};

    void moveInPlaneXZ(GLFWwindow* window, float dt, glm::vec3& cameraPos, glm::vec3& cameraRot);

    KeyMappings keys{};
    float moveSpeed{ 3.f };
    float lookSpeed{ 0.5f };
};