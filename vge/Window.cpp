#include "Window.h"

#include <stdexcept>

namespace vge
{
	VgeWindow::VgeWindow(int width, int height, std::string windowTitle) : width(width),
																		   height(height),
																		   windowTitle(windowTitle)
	{
		InitWindow();
	}

	VgeWindow::~VgeWindow()
	{
		glfwDestroyWindow(glfWindow);
		glfwTerminate();
	}

	void VgeWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
	{
		if (glfwCreateWindowSurface(instance, glfWindow, nullptr, surface) != VK_SUCCESS)
			throw std::runtime_error("failed to craete window surface");
	}

	VkExtent2D VgeWindow::GetExtent()
	{
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	bool VgeWindow::ShouldClose()
	{
		return glfwWindowShouldClose(glfWindow);
	}

	void VgeWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		glfwSetErrorCallback([](int error, const char* description)
		{
			fputs(description, stderr);
		});

		glfWindow = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
	}
}