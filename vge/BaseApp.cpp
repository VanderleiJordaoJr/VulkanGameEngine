#include "BaseApp.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>

namespace vge
{
	struct PushConstantData
	{
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	BaseApp::BaseApp()
	{
		LoadGameObjects();
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
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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

	void BaseApp::LoadGameObjects()
	{
		std::vector<VgeModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}};
		auto vgeModel = std::make_shared<VgeModel>(vgeDevice, vertices);

		auto triangle = VgeGameObject::CreateGameObject();
		triangle.model = vgeModel;
		triangle.color = {.1f, .8f, .1f};
		triangle.transform2d.rotation = .25 * glm::two_pi<float>();
		triangle.transform2d.scale = {.5f, .5f};
		triangle.transform2d.translation.x = 0.f;
		gameObjects.push_back(std::move(triangle));
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
		clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
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

		RenderGameObjects(commandBuffers[imageIndex]);

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

	void BaseApp::RenderGameObjects(VkCommandBuffer commandBuffer)
	{
		vgePipeline->Bind(commandBuffer);

		for (auto &gameObject : gameObjects)
		{
			gameObject.transform2d.rotation =
				glm::mod(gameObject.transform2d.rotation + 0.01f, glm::two_pi<float>());

			PushConstantData push{};
			push.offset = gameObject.transform2d.translation;
			push.color = gameObject.color;
			push.transform = gameObject.transform2d.Mat2();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PushConstantData),
				&push);
			gameObject.model->Bind(commandBuffer);
			gameObject.model->Draw(commandBuffer);
		}
	}
}