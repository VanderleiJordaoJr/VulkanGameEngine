#pragma once

#include "Device.h"
#include "GameObject.h"
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
		void FreeCommandBuffers();
		void LoadGameObjects();
		void RecordCommandBuffer(int imageIndex);
		void RecreateSwapChain();
		void RenderGameObjects(VkCommandBuffer commandBuffer);

		VgeWindow vgeWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
		VgeDevice vgeDevice{vgeWindow};
		std::unique_ptr<VgeSwapChain> vgeSwapChain;
		std::unique_ptr<VgePipeline> vgePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::vector<VgeGameObject> gameObjects;
	};
}
