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

// Simple framebuffer size callback. Keeps viewport synchronized with window size.
// Registered in initGlad().
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Constructor: stores the path/size parameters for the SRTM data/view.
SrtmView::SrtmView(const std::string filepath, const int size)
	: filepath(filepath), size(size) {
}

/*
 SrtmView::showSrtmData
 - High-level responsibilities:
    1. Initialize a GLFW window and OpenGL (GLAD).
    2. Initialize ImGui (one-time).
    3. Validate vertex data passed from caller.
    4. Upload vertex data to GPU (VAO/VBO).
    5. Create/prepare Shader.
    6. Enter the render loop:
         - Poll events
         - Render 3D scene (here: points)
         - Per-frame ImGui begin/NewFrame -> build UI -> Render ImGui
         - Handle UI actions (menu requests)
    7. Cleanup GPU/ImGui/GLFW resources.
 - Notes:
    - ImGui initialization is done once before the render loop.
    - Per-frame ImGui lifecycle (NewFrame/Render) must be executed every frame inside the render loop.
    - The example uses an orthographic projection and draws the vertex array as GL_POINTS.
*/
int SrtmView::showSrtmData(const std::vector<float>& vertices)
{
    // --------------------- GLFW INIT -------------------------
    // Create a window and OpenGL context
    GLFWwindow* window = initGlfWindow();
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        return -1;
    }

    // --------------------- GLAD INIT -------------------------
    // Initialize GL function pointers and basic state
    bool retFlag;
    int retVal = initGlad(window, retFlag);
    if (retFlag) return retVal;

    // --------------------- ImGui INIT -------------------------
    // Create ImGui context and set up style + backend bindings once.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io; // you can use 'io' to tweak keyboard/gamepad/cfg settings

    // Setup Dear ImGui style (colors)
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings (GLFW + OpenGL3)
    // The GLSL version string must match the context/profile (here "#version 330").
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // --------------------- Check VERTEX DATA -------------------------
    // Validate incoming vertex buffer: expect X,Y,Z triples.
    if (vertices.empty() || vertices.size() % 3 != 0) {
        std::cerr << "Invalid vertex data\n";
        // If invalid, perform ImGui + window cleanup before returning.
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Number of points (each vertex = 3 floats)
    size_t vertexCount = vertices.size() / 3;

    // --------------------- CREATE VAO/VBO -------------------------
    // Upload vertex data once to GPU (static).
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind and fill buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    // Vertex attribute: position at layout location 0 (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO for safety
    glBindVertexArray(0);

    // --------------------- SHADER -------------------------
    // Example shader used to render points. Make sure the files exist and compile.
    Shader shader("shaders/vertex.glsl", "shaders/fragment.glsl");

    // Menu action state: a simple flag set by ImGui menu item.
    // Flip to 'true' when user selects Tools -> AddFlightLine.
    bool addFlightLineRequested = false;

    // --------------------- RENDER LOOP -------------------------
    // Main loop: render scene and UI every frame until window close.
    while (!glfwWindowShouldClose(window)) {
        // Poll OS/window events first so ImGui IO is up-to-date
        glfwPollEvents();

        // ---- 3D Scene rendering ----
        // Clear color + depth buffers, enable depth testing.
        glClearColor(0.1f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // Use shader, set minimal uniforms (projection/modelView) and draw points
        shader.use();
        // Using orthographic projection for this example; replace with proper projection if needed.
        glm::mat4 projection = glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f);
        glm::mat4 modelView = glm::mat4(1.0f);
        shader.setMat4("uProjection", projection);
        shader.setMat4("uModelView", modelView);

        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, vertexCount);
        glBindVertexArray(0);

        // --------------------- ImGui per-frame -------------------------
        // Important: call NewFrame for both backends and ImGui every frame.
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Build your UI here:
        // - Example: a main menu bar, with a Tools menu and an AddFlightLine item.
        // - Selecting the menu item sets addFlightLineRequested = true.
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Add FlightLine")) {
                    addFlightLineRequested = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Finalize ImGui and render its draw lists on top of the scene.
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front/back buffers
        glfwSwapBuffers(window);

        // ---- Handle menu action outside immediate UI code ----
        // The menu action is handled here; for production code you may trigger a
        // callback or enqueue an action for the main app logic to perform.
        if (addFlightLineRequested) {
            std::cout << "Add FlightLine menu selected\n";
            // TODO: replace with actual flight line creation logic.
            addFlightLineRequested = false; // reset or keep as needed for your workflow
        }
    }

    // --------------------- CLEANUP -------------------------
    // Delete GPU resources
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    // Shutdown ImGui and free context (reverse order of initialization)
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Destroy window and terminate GLFW
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
    return 0;
}

/*
 initGlad
 - Loads GL function pointers through GLAD.
 - Registers the framebuffer size callback.
 - Sets initial viewport and some GL state (depth test, program point size).
 - Returns 0 on success, sets retFlag to false. On error sets retFlag true and returns negative value.
*/
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

/*
 initGlfWindow
 - Initializes GLFW (if not already) and creates a window + OpenGL context.
 - Configures the requested OpenGL version/profile via glfwWindowHint.
 - Returns a pointer to the created GLFWwindow or nullptr on failure.
 - Caller is responsible for making the context current before calling GL functions;
   this function calls glfwMakeContextCurrent(window) so GL calls may assume a current context.
*/
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

    // Make the context current so GL function loader and calls are valid.
    glfwMakeContextCurrent(window);

    return window;
}