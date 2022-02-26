#pragma once

#include "Device.h"
#include "Model.h"
#include "Pipeline.h"
#include "SwapChain.h"
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
		void CreateCommandBuffers();
		void CreatePipeline();
		void CreatePipelineLayout();
		void DrawFrame();
		void LoadModels();

		VgeWindow vgeWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
		VgeDevice vgeDevice{vgeWindow};
		VgeSwapChain vgeSwapChain{vgeDevice, vgeWindow.GetExtent()};
		std::unique_ptr<VgePipeline> vgePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<VgeModel> vgeModel;
	};
}
