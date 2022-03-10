#include "BaseApp.h"

// std
#include <array>
#include <stdexcept>

namespace vge
{

	BaseApp::BaseApp()
	{
		LoadModels();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	BaseApp::~BaseApp()
	{
		vkDestroyPipelineLayout(vgeDevice.device(), pipelineLayout, nullptr);
	}

	void BaseApp::Run()
	{
		while (!vgeWindow.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}

		vkDeviceWaitIdle(vgeDevice.device());
	}

	void BaseApp::CreateCommandBuffers()
	{
		commandBuffers.resize(vgeSwapChain->imageCount());

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

	void BaseApp::CreatePipeline()
	{
		assert(vgeSwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VgePipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = vgeSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vgePipeline = std::make_unique<VgePipeline>(
			vgeDevice,
			"shaders/Shader.vert.spv",
			"shaders/Shader.frag.spv",
			pipelineConfig);
	}

	void BaseApp::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(vgeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void BaseApp::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = vgeSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		RecordCommandBuffer(imageIndex);

		result = vgeSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || result == vgeWindow.WasWindowResized())
		{
			vgeWindow.ResetResizedFlag();
			RecreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	void BaseApp::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(
			vgeDevice.device(),
			vgeDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void BaseApp::LoadModels()
	{
		std::vector<VgeModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
		vgeModel = std::make_unique<VgeModel>(vgeDevice, vertices);
	}

	void BaseApp::RecordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = vgeSwapChain->getRenderPass();
		renderPassInfo.framebuffer = vgeSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = vgeSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(vgeSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(vgeSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{{0, 0}, vgeSwapChain->getSwapChainExtent()};
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		vgePipeline->Bind(commandBuffers[imageIndex]);
		vgeModel->Bind(commandBuffers[imageIndex]);
		vgeModel->Draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void BaseApp::RecreateSwapChain()
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
			vgeSwapChain = std::make_unique<VgeSwapChain>(vgeDevice, extent, std::move(vgeSwapChain));
			if (vgeSwapChain->imageCount() != commandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}

		CreatePipeline();
	}
}