#pragma once

#include "Device.h"

// std
#include <string>
#include <vector>

namespace vge
{

	struct PipelineConfigInfo
	{
		PipelineConfigInfo(const PipelineConfigInfo &) = delete;
		PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class VgePipeline
	{
	public:
		VgePipeline(
			VgeDevice &device,
			const std::string &vertFilepath,
			const std::string &fragFilepath,
			const PipelineConfigInfo &configInfo);
		~VgePipeline();
		VgePipeline(const VgePipeline &) = delete;
		void operator=(const VgePipeline &) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultPipelineConfigInfo(
			PipelineConfigInfo &configInfo, uint32_t width, uint32_t height);

	private:
		void CreateGraphicsPipeline(
			const std::string &vertFilepath,
			const std::string &fragFilepath,
			const PipelineConfigInfo &configInfo);
		void CreateShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);
		static std::vector<char> ReadFile(const std::string &filepath);

		VgeDevice &vgeDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}