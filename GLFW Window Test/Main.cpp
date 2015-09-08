#pragma once
#include "GraphicsWindow.h"

int main()
{
	setErrorCallbackAndInit(error_callback);
	GraphicsWindow window(1200,800,"Test Window",NULL,NULL,key_callback);
	window.makeContextCurrent();

	// Render Loop
	while(!window.shouldClose())
	{
		
		window.swapBuffers();

		glfwPollEvents();
	}

	window.release();
	return 0;
}