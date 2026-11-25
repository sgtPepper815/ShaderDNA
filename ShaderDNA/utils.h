#pragma once
#include <GLFW/glfw3.h>

double getDeltaTime(double lastTime, double phase)
{
	const double now = glfwGetTime();
	const double deltaTime = now - lastTime;
	lastTime = now;

	return deltaTime;
}