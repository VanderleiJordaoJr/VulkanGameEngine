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

		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class VgePipeline
	{
	public:
		VgePipeline() = default;
		VgePipeline(
			VgeDevice &device,
			const std::string &vertFilepath,
			const std::string &fragFilepath,
			const PipelineConfigInfo &configInfo);
		~VgePipeline();
		VgePipeline(const VgePipeline &) = delete;
		VgePipeline operator=(const VgePipeline &) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		static void DefaultPipelineConfigInfo(
			PipelineConfigInfo &configInfo);

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