#include "Renderer.h"

#include <array>
#include <cassert>
#include <stdexcept>

namespace vge
{
	VgeRenderer::VgeRenderer(VgeWindow &vgeWindow, VgeDevice &vgeDevice)
		: vgeWindow{vgeWindow}, vgeDevice{vgeDevice}
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	VgeRenderer::~VgeRenderer()
	{
		FreeCommandBuffers();
	}

	bool VgeRenderer::IsFrameInProgress() const
	{
		return isFrameStarted;
	};

	float VgeRenderer::GetAspectRatio() const
	{
		return vgeSwapChain->extentAspectRatio();
	};

	VkCommandBuffer VgeRenderer::GetCurrentCommandBuffer() const
	{
		assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
		return commandBuffers[currentFrameIndex];
	};

	int VgeRenderer::GetFrameIndex() const
	{
		assert(isFrameStarted && "Cannot get currentFrameIndex when frame not in progress");
		return currentFrameIndex;
	};

	VkRenderPass VgeRenderer::GetSwapChainRenderPass() const
	{
		return vgeSwapChain->getRenderPass();
	};

	VkCommandBuffer VgeRenderer::BeginFrame()
	{
		assert(!isFrameStarted && "Can't call BeginFrame while already in progress");

		auto result = vgeSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire swap chain image!");

		isFrameStarted = true;

		auto commandBuffer = GetCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer!");

		return commandBuffer;
	};

	void VgeRenderer::EndFrame()
	{
		assert(isFrameStarted && "Can't call EndFrame while frame is not in progress");

		auto commandBuffer = GetCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer!");

		auto result = vgeSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			vgeWindow.WasWindowResized())
		{
			vgeWindow.ResetResizedFlag();
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("failed to present swap chain image!");

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % VgeSwapChain::MAX_FRAMES_IN_FLIGHT;
	};

	void VgeRenderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == GetCurrentCommandBuffer() && "Can't call BeginSwapChainRenderPass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vgeSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vgeSwapChain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = vgeSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vgeSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vgeSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, vgeSwapChain->getSwapChainExtent()};
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	};

	void VgeRenderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
	{
		assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(
			commandBuffer == GetCurrentCommandBuffer() &&
			"Can't end render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	};

	void VgeRenderer::CreateCommandBuffers()
	{
		commandBuffers.resize(VgeSwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = vgeDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(vgeDevice.device(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void VgeRenderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(
			vgeDevice.device(),
			vgeDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void VgeRenderer::RecreateSwapChain()
	{
		auto extent = vgeWindow.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = vgeWindow.GetExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(vgeDevice.device());

		if (vgeSwapChain == nullptr)
		{
			vgeSwapChain = std::make_unique<VgeSwapChain>(vgeDevice, extent);
		}
		else
		{
			std::shared_ptr<VgeSwapChain> oldSwapChain = std::move(vgeSwapChain);

			vgeSwapChain = std::make_unique<VgeSwapChain>(vgeDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapChainFormat(*vgeSwapChain.get()))
				throw std::runtime_error("Swap chain image or depth format has changed!");
		}

		// TODO
	}
}