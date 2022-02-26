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
		CreatePipeline();
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
		commandBuffers.resize(vgeSwapChain.imageCount());

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

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vgeSwapChain.getRenderPass();
			renderPassInfo.framebuffer = vgeSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = vgeSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			vgePipeline->Bind(commandBuffers[i]);
			vgeModel->Bind(commandBuffers[i]);
			vgeModel->Draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void BaseApp::CreatePipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		VgePipeline::DefaultPipelineConfigInfo(
			pipelineConfig,
			vgeSwapChain.width(),
			vgeSwapChain.height());
		pipelineConfig.renderPass = vgeSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		vgePipeline = std::make_unique<VgePipeline>(
			vgeDevice,
			"../shaders/Shader.vert.spv",
			"../shaders/Shader.frag.spv",
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
		auto result = vgeSwapChain.acquireNextImage(&imageIndex);
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		result = vgeSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	void BaseApp::LoadModels()
	{
		std::vector<VgeModel::Vertex> vertices{{{0.0f, -0.5f}}, {{0.5f, 0.5f}}, {{-0.5f, 0.5f}}};
		vgeModel = std::make_unique<VgeModel>(vgeDevice, vertices);
	}
}