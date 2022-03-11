#pragma once

#include "Camera.h"
#include "Device.h"
#include "GameObject.h"
#include "Pipeline.h"

#include <memory>
#include <vector>

namespace vge
{
	class RenderSystem
	{
	public:
		RenderSystem(VgeDevice &vgeDevice, VkRenderPass renderPass);
		~RenderSystem();
		RenderSystem(const RenderSystem &) = delete;
		RenderSystem &operator=(const RenderSystem &) = delete;

		void RenderGameObjects(
			VkCommandBuffer commandBuffer,
			std::vector<VgeGameObject> &gameObjects,
			const VgeCamera &camera);

	private:
		void CreatePipeline(VkRenderPass renderPass);
		void CreatePipelineLayout();

		VgeDevice &vgeDevice;

		std::unique_ptr<VgePipeline> vgePipeline;
		VkPipelineLayout pipelineLayout;
	};
}
