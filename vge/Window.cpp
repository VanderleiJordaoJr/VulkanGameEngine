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
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
	}

	void VgeWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
	{
		if (glfwCreateWindowSurface(instance, glfwWindow, nullptr, surface) != VK_SUCCESS)
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
		return glfwWindowShouldClose(glfwWindow);
	}

	bool VgeWindow::WasWindowResized()
	{
		return frameBufferResized;
	}

	GLFWwindow* VgeWindow::GetGLFWWindow()
	{
		return glfwWindow;
	}

	void VgeWindow::FrameBufferResizeCallback(GLFWwindow *glfwWindow, int width, int height)
	{
		auto vgeWindow = reinterpret_cast<VgeWindow *>(glfwGetWindowUserPointer(glfwWindow));
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

		glfwWindow = glfwCreateWindow(width, height, windowTitle.c_str(), nullptr, nullptr);
		
		glfwSetWindowUserPointer(glfwWindow, this);
		glfwSetFramebufferSizeCallback(glfwWindow, FrameBufferResizeCallback);
	}
}