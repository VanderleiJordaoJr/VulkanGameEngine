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
			throw std::runtime_error("failed to create window surface");
	}

	VkExtent2D VgeWindow::GetExtent()
	{
		return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
	}

	void VgeWindow::ResetResizedFlag()
	{
		frameBufferResized = false;
	}

	bool VgeWindow::ShouldClose()
	{
		return glfwWindowShouldClose(glfWindow);
	}

	bool VgeWindow::WasWindowResized()
	{
		return frameBufferResized;
	}

	void VgeWindow::FrameBufferResizeCallback(GLFWwindow *glfWindow, int width, int height)
	{
		auto vgeWindow = reinterpret_cast<VgeWindow *>(glfwGetWindowUserPointer(glfWindow));
		vgeWindow->frameBufferResized = true;
		vgeWindow->width = width;
		vgeWindow->height = height;
	}

	void VgeWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		glfwSetErrorCallback([](int error, const char *description)
							 { fputs(description, stderr); });

		glfWindow = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
		
		glfwSetWindowUserPointer(glfWindow, this);
		glfwSetFramebufferSizeCallback(glfWindow, FrameBufferResizeCallback);
	}
}