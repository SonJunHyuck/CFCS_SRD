#include "context.h"

#include <spdlog/spdlog.h>
#include <glad/glad.h> // glfw 이전에 include!
#include <GLFW/glfw3.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

float DPI_WEIGHT = 1.0f;

void OnFramebufferSizeChange(GLFWwindow *window, int width, int height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    
    // reinterpret_cast : 아예 형변환 시켜버릴 것
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));
    context->Reshape(width, height);
}   

void OnKeyEvent(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
                key, scancode,
                action == GLFW_PRESS ? "Pressed" : 
                action == GLFW_RELEASE ? "Released" :
                action == GLFW_REPEAT ? "Repeat" : "Unknown",
                mods & GLFW_MOD_CONTROL ? "C" : "-",
                mods & GLFW_MOD_SHIFT ? "S" : "-",
                mods & GLFW_MOD_ALT ? "A" : "-");


    auto context = (Context *)glfwGetWindowUserPointer(window);
    context->KeyboardInput(key, action);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnCursorPos(GLFWwindow *window, double x, double y)
{
    auto context = (Context *)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow *window, int button, int action, int modifier)
{
    // 1.82 버전 이후, 마우스 인풋 작동
     ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context *)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnCharEvent(GLFWwindow *window, unsigned int ch)
{
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow *window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

int main()
{
    SPDLOG_INFO("Start program!");

    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit())
    {
        const char *description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);

        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SPDLOG_INFO("Create window!");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window)
    {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }

    // window가 만들어지면서, 만들어진 context를 사용하겠다.
    glfwMakeContextCurrent(window);

    // glad를 이용한 openGL 함수 로딩 (process address를 얻어옴)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = (const char *)glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);

    // ========== Glad Load 끝 -> 여기서부터 GL Functions 사용 가능 ==========

    // imgui
    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, false);  // callback setting false
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    auto context = Context::Create();
    if (!context)
    {
        SPDLOG_ERROR("failed to create context");   
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, context.get());

    // setting events
    // forced set viewport (frambuffer이 계속 window size * 2로 잡혀서, 강제로 버퍼 사이즈 설정 -> DPI problem)
    int Dpi_Width, Dpi_Height = 0;
    glfwGetFramebufferSize(window, &Dpi_Width, &Dpi_Height);

    DPI_WEIGHT = Dpi_Width / WINDOW_WIDTH;
    SPDLOG_INFO("DPI_WEIGHT : {}", DPI_WEIGHT);
    
    OnFramebufferSizeChange(window, WINDOW_WIDTH * DPI_WEIGHT, WINDOW_HEIGHT * DPI_WEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCharCallback(window, OnCharEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScroll);

    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window))
    {
        // Collecting mouse, keyboard, window size event... 
        glfwPollEvents();

        // Notify to ImGUI : "Start New Rendering Frame!"
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        context->Update();

        context->ProcessInput(window);
        context->Render();

        // Collecting Rendering Data
        ImGui::Render();
        
        // Real Draw
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // gl 끝나기 전에 메모리 정리 해야함
    context.reset();

    // Destroy : imgui (init 역순)
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwTerminate();

    return 0;
}