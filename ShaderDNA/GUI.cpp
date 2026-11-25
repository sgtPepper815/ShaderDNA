#include "GUI.h"

GUI::GUI(GLFWwindow* window)
{ 
    // Der OpenGL-Kontext muss hier bereits aktiv sein.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	setupTheme();
	loadFont();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

GUI::~GUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void GUI::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void GUI::Draw(std::array<float, 3>& bg, Shaders &shaders, fbos &framebuffers, unsigned int VAO)
{
    DrawAttributeWindow(bg);
	DrawMainViewport(bg, shaders.mainShader, 
		framebuffers.mainView.width, 
		framebuffers.mainView.height, 
		framebuffers.mainView.texture,
		framebuffers.mainView.fbo,
		VAO);

	DrawSecondViewport(bg, shaders, framebuffers, VAO);
}

void GUI::Render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUI::DrawToTexture(GLint fboTexture)
{
    ImVec2 size = ImGui::GetContentRegionAvail();
}

void GUI::DrawAttributeWindow(std::array<float, 3> &bg)
{
	ImGuiViewport* vp = ImGui::GetMainViewport();

	ImVec2 pos = vp->WorkPos;
	ImVec2 size = vp->WorkSize;

	float settingsWidth = size.x * 0.25f;
	float viewportWidth = size.x - settingsWidth;

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;

	// SETTINGS VIEWPORT
	ImGui::SetNextWindowPos({ pos.x, pos.y });
	ImGui::SetNextWindowSize(
		ImVec2(settingsWidth, size.y)
	);

    ImGui::Begin("Settings", nullptr, flags);

    ImGui::ColorEdit3("Background", bg.data());

    if (ImGui::Button("Click me!"))
        ++clicks;

    ImGui::Text("Clicks: %d", clicks);
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y));
	ImGui::SetNextWindowSize(ImVec2(settingsWidth, size.y));

	// SHADER
	ImGui::TextDisabled("SHADER");
	ImGui::Separator();
	ImGui::Spacing();

	//ImGui::SliderFloat("Speed", &speed, 0.0f, 5.0f);
	//ImGui::SliderFloat("Intensity", &intensity, 0.0f, 2.0f);

	ImGui::Spacing();
	ImGui::Separator();

	// PERFORMANCE
	ImGui::TextDisabled("PERFORMANCE");
	ImGui::Separator();

	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

    ImGui::End();
}

void GUI::DrawMainViewport(std::array<float, 3> &bg, GLuint &shader, 
	int &fboWidth, int &fboHeight, GLuint &fboTexture, GLuint &fbo, unsigned int &VAO)
{
	ImGuiViewport* vp = ImGui::GetMainViewport();

	ImVec2 pos = vp->WorkPos;
	ImVec2 size = vp->WorkSize;

	float settingsWidth = size.x * 0.25f;
	float secondViewportHeight = size.y * 0.50f;
	float viewportWidth = size.x - settingsWidth;

	ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;

	// MAIN VIEWPORT
	ImGui::SetNextWindowPos({ pos.x + settingsWidth, pos.y });
	ImGui::SetNextWindowSize(
		ImVec2(viewportWidth, size.y - secondViewportHeight)
	);

	ImGui::Begin("Main Viewport", nullptr, flags);

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	int renderWidth = static_cast<int>(viewportSize.x);
	int renderHeight = static_cast<int>(viewportSize.y);

	if (renderWidth > 0 && renderHeight > 0)
	{
		// Framebuffer bei Größenänderung anpassen
		if (renderWidth != fboWidth || renderHeight != fboHeight)
		{
			fboWidth = renderWidth;
			fboHeight = renderHeight;

			glBindTexture(GL_TEXTURE_2D, fboTexture);

			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RGBA8,
				fboWidth, fboHeight, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, nullptr
			);
		}

		// BEGIN MAIN RENDERING
		// render in the own framebuffer instead of the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glViewport(0, 0, fboWidth, fboHeight);

		glClearColor(
			bg[0],
			bg[1],
			bg[2],
			1.0f
		);

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glUniform2f(
			glGetUniformLocation(shader, "uResolution"),
			static_cast<float>(fboWidth),
			static_cast<float>(fboHeight)
		);

		glUniform1f(
			glGetUniformLocation(shader, "uTime"),
			static_cast<float>(glfwGetTime())
		);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// back to the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// display framebuffer texture in ImGui
		ImGui::Image(
			(ImTextureID)(intptr_t)fboTexture,
			viewportSize,
			ImVec2(0, 1),
			ImVec2(1, 0)
		);
	}

	ImGui::End();
	// END IMGUI
}

void GUI::DrawSecondViewport(
    std::array<float, 3>& bg, Shaders& shaders,
    fbos& framebuffers, unsigned int& VAO)
{

    ImGuiViewport* vp = ImGui::GetMainViewport();

    const float settingsWidth = vp->WorkSize.x * 0.25f;
    const float viewportHeight = vp->WorkSize.y * 0.5f;

    ImGui::SetNextWindowPos(ImVec2(
        vp->WorkPos.x + settingsWidth, vp->WorkPos.y + viewportHeight));
    ImGui::SetNextWindowSize(ImVec2(
        vp->WorkSize.x - settingsWidth, viewportHeight));

    const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

    if (!ImGui::Begin("Shader View", nullptr, flags))
    {
        ImGui::End();
        return;
    }

    const ImVec2 available = ImGui::GetContentRegionAvail();
    const ImVec2 spacing = ImGui::GetStyle().ItemSpacing;
    const float childWidth = (available.x - 2.0f * spacing.x) / 3.0f;
    const float childHeight = (available.y - spacing.y) / 2.0f;

    if (childWidth <= 0.0f || childHeight <= 0.0f)
    {
        ImGui::End();
        return;
    }

    const GLuint programs[] = {
        shaders.parentShader_1, shaders.parentShader_2,
        shaders.parentShader_3, shaders.parentShader_4,
        shaders.parentShader_5, shaders.parentShader_6
    };

    Framebuffer* targets[] = {
        &framebuffers.secondView, &framebuffers.thirdView,
        &framebuffers.fourthView, &framebuffers.fifthView,
        &framebuffers.sixthView, &framebuffers.seventhView
    };

    for (int i = 0; i < 6; ++i)
    {
        ImGui::PushID(i);

        const bool visible = ImGui::BeginChild(
            "Shader", ImVec2(childWidth, childHeight), ImGuiChildFlags_Borders);

        const ImVec2 imageSize = ImGui::GetContentRegionAvail();

        if (visible && imageSize.x >= 1.0f && imageSize.y >= 1.0f)
        {
            auto& target = *targets[i];
            const GLuint program = programs[i];
            const int width = static_cast<int>(imageSize.x);
            const int height = static_cast<int>(imageSize.y);

            if (program == 0 || target.fbo == 0 || target.texture == 0)
            {
                ImGui::Text("Parent %d: nicht initialisiert", i + 1);
            }
            else
            {
                if (target.width != width || target.height != height)
                {
                    target.width = width;
                    target.height = height;
                    glBindTexture(GL_TEXTURE_2D, target.texture);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                        width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                }

                glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
                glViewport(0, 0, width, height);
                glClearColor(bg[0], bg[1], bg[2], 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                glUseProgram(program);

                glUniform2f(glGetUniformLocation(program, "uResolution"),
                    static_cast<float>(width), static_cast<float>(height));

                glUniform1f(glGetUniformLocation(program, "uTime"),
                    static_cast<float>(glfwGetTime()));

                glBindVertexArray(VAO);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);

                ImGui::Image((ImTextureID)(intptr_t)target.texture, imageSize,
                    ImVec2(0, 1), ImVec2(1, 0));
            }
        }
        ImGui::EndChild();
        ImGui::PopID();
        if (i % 3 != 2)
            ImGui::SameLine();
    }
    ImGui::End();
}

void GUI::setupTheme()
{
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	// Rounding
	style.WindowRounding = 8.0f;
	style.ChildRounding = 6.0f;
	style.FrameRounding = 5.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 8.0f;
	style.GrabRounding = 5.0f;
	style.TabRounding = 5.0f;

	// Padding
	style.WindowPadding = ImVec2(14, 14);
	style.FramePadding = ImVec2(10, 6);
	style.ItemSpacing = ImVec2(10, 10);
	style.ItemInnerSpacing = ImVec2(8, 6);

	//Borders
	style.WindowBorderSize = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.PopupBorderSize = 1.0f;

	// Scrollbar
	style.ScrollbarSize = 10.0f;
	style.GrabMinSize = 10.0f;

	// Colors
	ImVec4* colors = style.Colors;

	colors[ImGuiCol_WindowBg] = ImVec4(0.075f, 0.080f, 0.105f, 1.0f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.095f, 0.100f, 0.130f, 1.0f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.110f, 0.115f, 0.145f, 1.0f);

	colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.96f, 1.0f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.48f, 0.50f, 0.57f, 1.0f);

	colors[ImGuiCol_Border] = ImVec4(0.20f, 0.22f, 0.28f, 1.0f);

	colors[ImGuiCol_FrameBg] = ImVec4(0.14f, 0.15f, 0.19f, 1.0f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.21f, 0.27f, 1.0f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.23f, 0.25f, 0.33f, 1.0f);

	// Buttons
	colors[ImGuiCol_Button] = ImVec4(0.28f, 0.35f, 0.85f, 1.0f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.36f, 0.44f, 0.98f, 1.0f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.22f, 0.29f, 0.72f, 1.0f);

	// Header
	colors[ImGuiCol_Header] = ImVec4(0.18f, 0.21f, 0.31f, 1.0f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.30f, 0.45f, 1.0f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.36f, 0.55f, 1.0f);

	// Titlebar
	colors[ImGuiCol_TitleBg] = ImVec4(0.065f, 0.070f, 0.095f, 1.0f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.095f, 0.105f, 0.145f, 1.0f);

	// Slider
	colors[ImGuiCol_SliderGrab] = ImVec4(0.38f, 0.47f, 0.95f, 1.0f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.48f, 0.56f, 1.0f, 1.0f);

	// Separator
	colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.22f, 0.28f, 1.0f);

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.13f, 0.17f, 1.0f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.30f, 0.55f, 1.0f);
	colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.25f, 0.43f, 1.0f);
	
}

void GUI::loadFont()
{
	std::filesystem::path fontPath =
		std::filesystem::path(FONT_DIRECTORY) /
		"Inter-4.1/InterVariable.ttf";

	std::cout << fontPath << std::endl;

	ImGuiIO& io = ImGui::GetIO();

	ImFont* font = io.Fonts->AddFontFromFileTTF(
		fontPath.string().c_str(),
		17.0f
	);

	if (font)
	{
		io.FontDefault = font;
	}
	else
	{
		std::cerr << "Could not load font!\n";
	}
}
