#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


const float S = 10.0, R = 28.0, B = 8/3;
inline void lorentzAttractor(float& x, float& y, float& z, float dt) {
    float dx = dt * (S * (y - x));
    float dy = dt * (x * (R - z) - y);
    float dz = dt * (x * y - B * z);
    x += dx;
    y += dy;
    z += dz;
}

inline void cosine(float& x, float& y, float& z, float dt) {
    x = 0.0;
    z += dt;
    y = cos(z);
}

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Functions Viewer", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shader programs
    Shader functionShader("func.vs", "func.fs");
    Shader axesShader("axes.vs", "axes.fs");

    // set up vertex data and buffers and configure vertex attributes

    // axes
    float axes[] = {
        00.00f, 00.00f, 00.00f, 1.00f, 0.00f, 0.00f,
        10.00f, 00.00f, 00.00f, 1.00f, 0.00f, 0.00f,

        00.00f, 00.00f, 00.00f, 0.00f, 1.00f, 0.00f,
        00.00f, 10.00f, 00.00f, 0.00f, 1.00f, 0.00f,

        00.00f, 00.00f, 00.00f, 0.00f, 0.00f, 1.00f,
        00.00f, 00.00f, 10.00f, 0.00f, 0.00f, 1.00f,
    };

    unsigned int axesVBO, axesVAO;
    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);
    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // function
    const int pointsQuantity = 5000;
    float x = 0.1f, y = 0.1f, z = 0.1f, dt = 0.01f; //change start point
    int verticesBufferSize = 3 * pointsQuantity * sizeof(float);
    float* vertices = (float*)malloc(verticesBufferSize);
    for (int i = 0; i < pointsQuantity; i++) {
        vertices[i * 3 + 0] = (float)x;
        vertices[i * 3 + 1] = (float)y;
        vertices[i * 3 + 2] = (float)z;
        lorentzAttractor(x,y,z,dt);  //change function
    }

    unsigned int VBO1, VAO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glBindVertexArray(VAO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, verticesBufferSize, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);

    glPointSize((GLfloat)3.0f); // change points size

    // render loop
    while (!glfwWindowShouldClose(window)) {

        // time logic
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // prepare transformations
        glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 model = glm::mat4(1.0f);

        // activate axesShader
        axesShader.use(); 
        axesShader.setMat4("projection", projection);
        axesShader.setMat4("view", view);
        axesShader.setMat4("model", model);

        // render axes
        glBindVertexArray(axesVAO);
        glDrawArrays(GL_LINES, 0, 6);

        // activate shader
        functionShader.use();
        functionShader.setMat4("projection", projection);
        functionShader.setMat4("view", view);
        functionShader.setMat4("model", model);

        // render boxes
        glBindVertexArray(VAO1);
        functionShader.setVec3("color", glm::vec3(1.0f, 1.0f, 0.0f));
        //glDrawArrays(GL_LINE_STRIP, 0, pointsQuantity); // use GL_LINE_STRIP to print lines or GL_POINTS to print points
        glDrawArrays(GL_POINTS, 0, pointsQuantity);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // deallocate resources
    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);

    // clear allocated GLFW resources.
    glfwTerminate();

    free(vertices);

    return 0;
}

// process input: keys that are pressed/released
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// if the window size changed this callback executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// if the mouse moves, this callback executes
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// if the mouse scroll wheel moves, this callback executes
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}