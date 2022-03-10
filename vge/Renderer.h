#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "Window.h"

#include <memory>
#include <vector>

namespace vge
{
	class VgeRenderer
	{
	public:
		VgeRenderer(VgeWindow &vgeWindow, VgeDevice &vgeDevice);
		~VgeRenderer();

		VgeRenderer(const VgeRenderer &) = delete;
		VgeRenderer &operator=(const VgeRenderer &) = delete;

		bool IsFrameInProgress() const;
		int GetFrameIndex() const;
		VkCommandBuffer GetCurrentCommandBuffer() const;
		VkRenderPass GetSwapChainRenderPass() const;

		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		VgeWindow &vgeWindow;
		VgeDevice &vgeDevice;
		std::unique_ptr<VgeSwapChain> vgeSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{0};
		bool isFrameStarted{false};
	};
}
