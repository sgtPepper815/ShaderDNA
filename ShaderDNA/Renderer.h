#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <vector>
#include <array>

#include "ShaderLoader.h"
#include "Genome.h"
#include "GUI.h"
#include "Shaders.h"


using QuadVertices = std::array<float, 12>;

class Renderer
{
public:

	void init();
	void run();
	void cleanup();

	static void processInput(GLFWwindow* window, int key, int scancode, int action, int mods);

	QuadVertices createScreenQuad(int size, int screen_width, int screen_height);

	unsigned int VBO, VAO, EBO;

	Shaders shaders;

	std::vector<DNA> population;
	void UploadGenomeToShader(GLuint program, const DNA& dna);

private:
	GLFWwindow* mWindow;
	std::unique_ptr<GUI> gui = nullptr;
	void setupShaderAndGeometry();
	void SetResolutionAndTimeUniforms(int width, int height);

	std::array<float, 3> background = { 0.12f, 0.18f, 0.25f };
	int clicks = 0;

	fbos framebuffers;
	void createMainViewFrameBuffer();
	void createSecondViewFrameBuffer();


};