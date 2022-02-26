#pragma once

#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace vge
{
	class VgeModel
	{
	public:
		struct Vertex
		{
			glm::vec2 position;
			glm::vec3 color;

			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		};

		VgeModel(VgeDevice &device, const std::vector<Vertex> &vertices);
		~VgeModel();
		VgeModel(const VgeModel &) = delete;
		VgeModel &operator=(const VgeModel &) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:
		void CreateVertexBuffers(const std::vector<Vertex> &vertices);

		VgeDevice &vgeDevice;
		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;
	};
}