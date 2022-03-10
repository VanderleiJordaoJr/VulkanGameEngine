#pragma once

#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Window.h"

#include <memory>
#include <vector>

namespace vge
{
	class BaseApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		BaseApp();
		~BaseApp();
		BaseApp(const BaseApp &) = delete;
		BaseApp &operator=(const BaseApp &) = delete;

		void Run();

	private:
		void LoadGameObjects();

		VgeWindow vgeWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
		VgeDevice vgeDevice{vgeWindow};
		VgeRenderer vgeRenderer{vgeWindow, vgeDevice};

		std::vector<VgeGameObject> gameObjects;
	};
}
