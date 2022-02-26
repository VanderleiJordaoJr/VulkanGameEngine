#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vge
{
	class VgeWindow
	{
	public:
		VgeWindow(int width, int height, std::string windowTitle);
		~VgeWindow();
		VgeWindow(const VgeWindow &) = delete;
		VgeWindow &operator=(const VgeWindow &) = delete;

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
		VkExtent2D GetExtent();
		bool ShouldClose();
	private:
		void InitWindow();
		
		const int width;
		const int height;
		std::string windowTitle;
		GLFWwindow *glfWindow;
	};
};