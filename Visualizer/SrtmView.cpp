#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "SrtmView.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

SrtmView::SrtmView(const std::string filepath, const int size)
	: filepath(filepath), size(size) {
}

int SrtmView::showSrtmData(const std::vector<float>& vertices)
{
    // --------------------- GLFW INIT -------------------------

    GLFWwindow* window = initGlfWindow();
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        return -1;
    }

    // --------------------- GLAD INIT -------------------------
    bool retFlag;
    int retVal = initGlad(window, retFlag);
    if (retFlag) return retVal;

    // --------------------- ImGui INIT -------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --------------------- Check VERTEX DATA -------------------------

    if (vertices.empty() || vertices.size() % 3 != 0) {
        std::cerr << "Invalid vertex data\n";
        // cleanup window and terminate
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    size_t vertexCount = vertices.size() / 3;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // --------------------- SHADER -------------------------
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // --------------------- RENDER LOOP -------------------------
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

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

    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
    return 0;
}

int SrtmView::initGlad(GLFWwindow* window, bool& retFlag)
{
    if (!window) {
        std::cerr << "initGlad called with null window\n";
        retFlag = true;
        return -1;
    }

    retFlag = true;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }

    // now safe to call GL functions and register callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST); // ensure depth buffer is meaningful
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(2.0f);
    retFlag = false;
    return 0;
}

GLFWwindow* SrtmView::initGlfWindow()
{

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return nullptr;
    }

    // Request a modern OpenGL context — adjust version/profile as needed
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "DEM Point Cloud", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    return window;
}