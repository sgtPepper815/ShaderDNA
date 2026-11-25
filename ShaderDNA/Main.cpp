
#include <iostream>

#include "Renderer.h"

int main()
{
	Renderer mRenderer;

	mRenderer.init();
	mRenderer.run();
	mRenderer.cleanup();
}
