#include "context.h"
#include "framework/image.h"
#include <imgui.h>
#include <iostream>

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow *window)
{
    // Camera Control
    if (m_camera.isControl)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            m_camera.Translate(m_camera.front);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            m_camera.Translate(-m_camera.front);

        auto cameraRight = glm::normalize(glm::cross(m_camera.up, -m_camera.front));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            m_camera.Translate(cameraRight);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            m_camera.Translate(-cameraRight);

        auto cameraUp = glm::normalize(glm::cross(-m_camera.front, cameraRight));
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            m_camera.Translate(cameraUp);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            m_camera.Translate(-cameraUp);
    }
}


void Context::KeyboardInput(int key, int action)
{
    // Draw Path
    if (!m_simulation->bIsDrawMode && !m_simulation->bIsSimulate)
    {
        for (int PRESS_KEY = GLFW_KEY_0; PRESS_KEY <= GLFW_KEY_9; PRESS_KEY++)
        {
            if (key == PRESS_KEY && action == GLFW_PRESS)
            {
                uint8_t DrawGroupId = std::min(PRESS_KEY - 48, m_simulation->GetNumGroups() - 1);
                //m_simulation->SetDrawPathId(PRESS_KEY - 48);  // GLFW_KEY_0 = 48
                m_simulation->DrawPathId = DrawGroupId;
            }
        }
    }

    // Simulate Mode
    if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
    {
        m_simulation->SwitchSimulate();
        //m_simulation->bIsSimulate = !m_simulation->bIsSimulate;
    }
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
    m_framebuffer = Framebuffer::Create( Texture::Create(width, height, GL_RGBA) );
}

void Context::MouseMove(double x, double y)
{
    if (m_camera.isControl)
    {        
        m_camera.Rotate(x, y);
    }

    if (m_simulation->bIsDrawMode && !m_simulation->bIsSimulate)
    {
        glm::vec3 FinalPos = m_camera.GetWorldPos(static_cast<float>(m_width), static_cast<float>(m_height), x, y);

        // SPDLOG_INFO("RayPos : ({}, {}, {})", FinalPos.x, FinalPos.y, FinalPos.z);

        m_simulation->DrawPath(FinalPos);

        std::vector<glm::vec3> TempPath = std::vector<glm::vec3>();
        m_simulation->GetWaypoints(TempPath);
        m_lines[m_simulation->DrawPathId]->Update(TempPath);
    }
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
            m_camera.prevMousePos = glm::vec2((float)x, (float)y);
            m_camera.isControl = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_camera.isControl = false;
        }
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if(action == GLFW_PRESS)
        {
            m_simulation->bIsDrawMode = true;
        }
        else if(action == GLFW_RELEASE)
        {
            m_simulation->bIsDrawMode = false;
        }
    }
}

bool Context::Init()
{
    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();
    m_lines.push_back(Line::Create());
    m_lines.push_back(Line::Create());
    
    // Checkboard
    float FieldSize = GRID_BOUND_X + GRID_BOUND_Z;
    //m_checkboard = CheckBoard::CreateCheckBoard(FieldSize, FieldSize / GRID_DENSITY);
    m_checkboard = CheckBoard::CreateCheckBoard(GRID_BOUND_X, GRID_BOUND_Z, FieldSize / GRID_DENSITY);

    // ========== Create & Attach & Link ========= (create shader func has covered program clas)
    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if (!m_simpleProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_simpleProgram->Get());

    m_lightProgram = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if (!m_lightProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_lightProgram->Get());

    m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");
    if (!m_textureProgram)
      return false;
    SPDLOG_INFO("program id: {}", m_textureProgram->Get());

    m_postProgram = Program::Create("./shader/texture.vs", "./shader/gamma.fs");
    if (!m_postProgram)
        return false;
    SPDLOG_INFO("program id: {}", m_postProgram->Get());

    // ======== Texture ========
    TexturePtr darkGrayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    TexturePtr whiteTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = grayTexture;
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;
    SPDLOG_INFO("Texture Load Success");

    // ======== Model(Mesh) ========
    // m_models.push_back(Model::Load("./models/backpack.obj"));

    // if (m_models[0] == nullptr)
    //     return false;
        
    // SPDLOG_INFO("Model Load Success");

    // ======== Formation ========

    // ======== Simulation ========
    // Input Group Count -> Group_1 AgentCount -> Group_1 Formation -> Group_2 AgentCount -> Group_2 Formation -> ...
    uint8_t NumGroups = 2;

    std::vector<uint32_t> Agents;
    Agents.push_back(100);
    Agents.push_back(100);

    std::vector<uint8_t> FormationIds;
    FormationIds.push_back(0);
    FormationIds.push_back(0);

    m_simulation = Simulation::Create(NumGroups, Agents);

    // Set Formation
    for(int i = 0; i < FormationIds.size(); i++)
    {
        uint8_t FormationId = FormationIds[i];
       // m_simulation->SetFormation(RectFormation->GetPositions(), i, VEC_ZERO, 1.0f);
    }

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    return true;
}

void Context::Update()
{
    m_simulation->Update();
}

void Context::Render()
{
    // begin, end pair -> UI Window #imgui
    if (ImGui::Begin("my first ImGui window"))
    {
        if(ImGui::ColorEdit4("Clear Color", glm::value_ptr(m_clearColor)))
        {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();
        
        ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
        ImGui::Separator();
        
        ImGui::DragFloat("Camera Speed", &m_camera.translateSpeed, 0.001f, 0.001f, 0.3f);
        ImGui::DragFloat("Camera Rotate Speed", &m_camera.rotateSpeed, 0.001f, 0.001f, 0.1f);
        ImGui::DragFloat3("Camera Pos", glm::value_ptr(m_camera.position), 0.01f);
        ImGui::DragFloat("Camera Yaw", &m_camera.yaw, 0.5f);
        ImGui::DragFloat("Camera Pitch", &m_camera.pitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();

        if(ImGui::Button("Reset Camera"))
        {
            m_camera.yaw = 0.0f;
            m_camera.pitch = 0.0f;
            m_camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
        }
        ImGui::Separator();

        // ImGui : Light & Material
        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 5.0f, 0.0f ,180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));

            ImGui::Checkbox("flash light", &m_flashLightMode);
        }

        ImGui::Checkbox("animation", &m_animation);

        ImGui::Text("This is first text...");
    }
    ImGui::End();

    m_framebuffer->Bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_camera.front = 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_camera.yaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_camera.pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);  // w : 1.0f -> 점, 0.0f -> 벡터 (위치 상관x -> 평행이동x)
    
    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 400.0f);
    auto view = glm::lookAt(m_camera.position, m_camera.position + m_camera.front, m_camera.up);

    // light cube
    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if (m_flashLightMode)
    {
        lightPos = m_camera.position;
        lightDir = m_camera.front;
    }
    else
    {
        // Render LightCube
        auto lightModelTransform =
            glm::translate(glm::mat4(1.0), m_light.position) * glm::scale(glm::mat4(1.0), glm::vec3(0.1f));
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
    }

    // Render Model
    m_lightProgram->Use();
    m_lightProgram->SetUniform("viewPos", m_camera.position);
    m_lightProgram->SetUniform("light.position", lightPos);
    m_lightProgram->SetUniform("light.direction", lightDir);
    m_lightProgram->SetUniform("light.cutoff", glm::vec2(
                                              cosf(glm::radians(m_light.cutoff[0])),
                                              cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_lightProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_lightProgram->SetUniform("light.ambient", m_light.ambient);
    m_lightProgram->SetUniform("light.diffuse", m_light.diffuse);
    m_lightProgram->SetUniform("light.specular", m_light.specular);

    m_simpleProgram->Use();
    auto modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    auto transform = projection * view * modelTransform;
    m_simpleProgram->SetUniform("transform", transform);
    m_simpleProgram->SetUniform("color", glm::vec4(1, 1, 1, 1));
    m_checkboard->Draw(m_simpleProgram.get());
    
    // Path
    for(uint8_t GroupId = 0; GroupId < m_simulation->GetNumGroups(); GroupId++)
    {
        modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        transform = projection * view * modelTransform;
        m_simpleProgram->SetUniform("transform", transform);
        m_simpleProgram->SetUniform("color", glm::vec4(m_simulation->GetGroupColor(GroupId), 1));

        if(m_lines[GroupId]->IsDrawable)
            m_lines[GroupId]->Draw(m_simpleProgram.get());
    }

    // Agents
    for(uint32_t AgentId = 0; AgentId < m_simulation->GetNumAgents(); AgentId++)
    {
        modelTransform = glm::translate(glm::mat4(1.0f), m_simulation->GetAgentPosition(AgentId));
        transform = projection * view * modelTransform;
        m_simpleProgram->SetUniform("transform", transform);
        m_simpleProgram->SetUniform("color", glm::vec4(m_simulation->GetGroupColor(AgentId), 1));
        m_box->Draw(m_simpleProgram.get());
    }

    // SRD
    // for(uint32_t AgentId = 0; AgentId < m_simulation->GetNumAgents(); AgentId++)
    // {
    //     modelTransform = glm::translate(glm::mat4(1.0f), m_simulation->GetAgentSRD(AgentId));
    //     transform = projection * view * modelTransform;
    //     m_simpleProgram->SetUniform("transform", transform);
    //     m_simpleProgram->SetUniform("color", glm::vec4(m_simulation->GetGroupColor(AgentId) * 0.7f, 1));
    //     m_box->Draw(m_simpleProgram.get());
    // }

    Framebuffer::BindToDefault();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    m_postProgram->Use();
    m_postProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));  // -1 ~ 1 (2) 까지의 범위를 모두 커버하기 위해
    m_framebuffer->GetColorAttachment()->Bind();
    m_postProgram->SetUniform("tex", 0);
    m_postProgram->SetUniform("gamma", m_gamma);
    m_plane->Draw(m_postProgram.get());
}