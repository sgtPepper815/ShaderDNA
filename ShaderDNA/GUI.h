#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Framebuffer.h"
#include "Shaders.h"

#include <array>
#include <filesystem>
#include <iostream>

class GUI
{
public:
	GUI(GLFWwindow* window);
	~GUI();

	void beginFrame();
	static void Draw(std::array<float, 3>& bg, Shaders &shaders, fbos &framebuffers, unsigned int VAO);
	void Render();

	void DrawToTexture(GLint fboTexture);

private:
	inline static int clicks = 0;

	static void DrawAttributeWindow(std::array<float, 3> &bg);
	static void DrawMainViewport(std::array<float, 3> &bg, GLuint &shader, 
		int &fboWidth, int &fboHeight, GLuint &fboTexture, GLuint &fbo, unsigned int &VAO);
	static void DrawSecondViewport(std::array<float, 3> &bg, Shaders& shaders,
		fbos &framebuffers, unsigned int& VAO);

	void setupTheme();
	void loadFont();



};