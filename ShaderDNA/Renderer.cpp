
#define GLFW_EXPOSE_NATIVE_WIN32

#include <iostream>

#include "Renderer.h"
#include "Genome.h"
#include "utils.h"

#include <dwmapi.h>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

constexpr int POPULATION_SIZE = 6;

void Renderer::init()
{
	if (!glfwInit()) 
	{
		std::cout << "Failed to init GLFW" << std::endl;
		cleanup();
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ShaderDNA", NULL, NULL);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		cleanup();
	}

	BOOL darkMode = TRUE;

	HRESULT result = DwmSetWindowAttribute(
		glfwGetWin32Window(mWindow),
		DWMWA_USE_IMMERSIVE_DARK_MODE,
		&darkMode,
		sizeof(darkMode)
	);

	if (FAILED(result))
		std::cerr << "Could not load dark mode.\n";

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	gui = std::make_unique<GUI>(mWindow);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});
	
	glfwSetKeyCallback(mWindow, processInput);

	setupShaderAndGeometry();

	createMainViewFrameBuffer();
	createSecondViewFrameBuffer();
}

// INIT SHADER AND GEOMETRY
void Renderer::setupShaderAndGeometry()
{
	QuadVertices verticesToDraw = createScreenQuad(10, WINDOW_WIDTH, WINDOW_HEIGHT);
	unsigned int indices[] = {
		0, 1, 2,
		0, 3, 1
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesToDraw), verticesToDraw.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	try
	{
		// loads shader.vert and shader.frag
		shaders.mainShader = ShaderLoader::load("shader");
		shaders.parentShader_1 = ShaderLoader::load("shader");
		shaders.parentShader_2 = ShaderLoader::load("shader");
		shaders.parentShader_3 = ShaderLoader::load("shader");
		shaders.parentShader_4 = ShaderLoader::load("shader");
		shaders.parentShader_5 = ShaderLoader::load("shader");
		shaders.parentShader_6 = ShaderLoader::load("shader");
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << '\n';
	}

	Genome::generateDNAPopulation(POPULATION_SIZE, &population);

	// create a child with crossover and mutation
	// mutationRate should be between 0% and 100%
	DNA child = Genome::createChild(&population, 0.35f, 2.0f);

	UploadGenomeToShader(shaders.mainShader, child);
	UploadGenomeToShader(shaders.parentShader_1, population[0]);
	UploadGenomeToShader(shaders.parentShader_2, population[1]);
	UploadGenomeToShader(shaders.parentShader_3, population[2]);
	UploadGenomeToShader(shaders.parentShader_4, population[3]);
	UploadGenomeToShader(shaders.parentShader_5, population[4]);
	UploadGenomeToShader(shaders.parentShader_6, population[5]);

}


void Renderer::run()
{
	while (!glfwWindowShouldClose(mWindow))
	{
		glfwPollEvents();

		// BEGIN IMGUI
		gui->beginFrame();
		gui->Draw(background, shaders, framebuffers, VAO);
		// END IMGUI

		// get the current window size (important for resizing)
		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);
		glViewport(0, 0, width, height);

		glClearColor(background[0], background[1], background[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// BEGIN MAIN RENDERING
		glUseProgram(shaders.mainShader);
		// END MAIN RENDERING
		
		// draw imgui
		gui->Render();

		glfwSwapBuffers(mWindow);
	}

	cleanup();
}

void Renderer::cleanup()
{
    if (!mWindow)
        return;

    gui.reset();
    GLuint programs[] = {
        shaders.mainShader, shaders.parentShader_1, shaders.parentShader_2,
        shaders.parentShader_3, shaders.parentShader_4,
        shaders.parentShader_5, shaders.parentShader_6
    };
    for (GLuint program : programs)
        glDeleteProgram(program);
    shaders = {};

    Framebuffer* targets[] = {
        &framebuffers.mainView, &framebuffers.secondView,
        &framebuffers.thirdView, &framebuffers.fourthView,
        &framebuffers.fifthView, &framebuffers.sixthView,
        &framebuffers.seventhView
    };
    for (auto* target : targets)
    {
        glDeleteTextures(1, &target->texture);
        glDeleteFramebuffers(1, &target->fbo);
        target->texture = target->fbo = 0;
    }
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glfwDestroyWindow(mWindow);
    mWindow = nullptr;
    glfwTerminate();
}

void Renderer::processInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

QuadVertices Renderer::createScreenQuad(int size, int screen_width, int screen_height)
{

	float _s = float(size) / 10;

	std::cout << _s << std::endl;

	// Create a quad
	QuadVertices vertices = {
	-_s, -_s, 0.0f,
	 _s, _s, 0.0f,
	 -_s, _s, 0.0f,
	 _s,  -_s, 0.0f
	};

	return vertices;
}

void Renderer::UploadGenomeToShader(GLuint program, const DNA& dna)
{
	glUseProgram(program);

	glUniform1f(glGetUniformLocation(program, "uFrequency"), dna.frequency);
	glUniform1f(glGetUniformLocation(program, "uDistortion"), dna.distortion);
	glUniform1f(glGetUniformLocation(program, "uSpeed"), dna.speed);
	glUniform3f(glGetUniformLocation(program, "uColorA"), dna.colorA.r, dna.colorA.g, dna.colorA.b);
	glUniform3f(glGetUniformLocation(program, "uColorB"), dna.colorB.r, dna.colorB.g, dna.colorB.b);
}

void Renderer::SetResolutionAndTimeUniforms(int width, int height)
{
	glUniform2f(
		glGetUniformLocation(shaders.mainShader, "uResolution"),
		static_cast<float>(width),
		static_cast<float>(height)
	);

	// subtract startTime to get the time since the program started
	// without it the time would be the time since the computer started
	glUniform1f(
		glGetUniformLocation(shaders.mainShader, "uTime"),
		static_cast<float>(glfwGetTime())
	);
}

void Renderer::createMainViewFrameBuffer()
{
	glGenFramebuffers(1, &framebuffers.mainView.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.mainView.fbo);

	glGenTextures(1, &framebuffers.mainView.texture);
	glBindTexture(GL_TEXTURE_2D, framebuffers.mainView.texture);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA8,
		framebuffers.mainView.width, framebuffers.mainView.height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, nullptr
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D,
		framebuffers.mainView.texture,
		0
	);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Framebuffer incomplete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::createSecondViewFrameBuffer()
{
    Framebuffer* targets[] = {
        &framebuffers.secondView, &framebuffers.thirdView,
        &framebuffers.fourthView, &framebuffers.fifthView,
        &framebuffers.sixthView, &framebuffers.seventhView
    };
    for (auto* target : targets)
    {
        glGenFramebuffers(1, &target->fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, target->fbo);
        glGenTextures(1, &target->texture);
        glBindTexture(GL_TEXTURE_2D, target->texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
            target->width, target->height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, target->texture, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("Parent framebuffer incomplete");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
