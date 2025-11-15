#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Simulator/DemTerrain.h"
#include "Shader.h"

int main() {
    const std::string filepath = "C:\\Dev\\LidarSimulator\\SRTM\\N33W118.hgt";
    const int size = 3601;

    DemTerrain dem(filepath, size);

    // --------------------- GLFW INIT -------------------------
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "DEM Point Cloud", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // --------------------- GLAD INIT -------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(1.0f);

    // --------------------- BUILD VERTEX DATA -------------------------

    std::vector<float> vertices = dem.getAllPoints();
    int vertexCount = vertices.size() / 6;

    // --------------------- CREATE VAO/VBO -------------------------
    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    // positions (layout = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // colors (layout = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // --------------------- SHADER -------------------------
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // --------------------- RENDER LOOP -------------------------
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
        glm::mat4 modelView = glm::mat4(1.0f);

        shader.setMat4("uProjection", projection);
        shader.setMat4("uModelView", modelView);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}
