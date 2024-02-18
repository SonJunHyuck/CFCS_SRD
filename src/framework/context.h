#pragma once

#include "../common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"
#include "camera.h"
#include "checkboard.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

private:
    Context() {}
    bool Init();

    // Shader Program
    ProgramUPtr m_lightProgram;
    ProgramUPtr m_simpleProgram;
    ProgramUPtr m_textureProgram;
    ProgramUPtr m_postProgram;

    float m_gamma {1.0f};

    // object
    MeshUPtr m_box;
    MeshUPtr m_plane;
    CheckBoardUPtr m_checkboard;

    // material
    MaterialPtr m_planeMaterial;

    // animation
    bool m_animation { true };

    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.3f, 0.3f, 0.3f, 0.0f) };

    // light parameter
    struct Light
    {
        glm::vec3 position{glm::vec3(1.0f, 4.0f, 4.0f)};
        glm::vec3 direction{glm::vec3(-1.0f, -1.0f, -1.0f)};
        glm::vec2 cutoff{glm::vec2(120.0f, 5.0f)};
        float distance{128.0f};
        glm::vec3 ambient{glm::vec3(0.1f, 0.1f, 0.1f)};
        glm::vec3 diffuse{glm::vec3(0.8f, 0.8f, 0.8f)};
        glm::vec3 specular{glm::vec3(1.0f, 1.0f, 1.0f)};
    };
    Light m_light;
    bool m_flashLightMode { false };

    // camera
    Camera m_camera;

    // framebuffer
    FramebufferUPtr m_framebuffer;

    // window
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };
};