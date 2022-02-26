#include "Model.h"

#include <cassert>
#include <cstring>

namespace vge
{
	std::vector<VkVertexInputAttributeDescription> VgeModel::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = 0;
		return attributeDescriptions;
	}

	std::vector<VkVertexInputBindingDescription> VgeModel::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	VgeModel::VgeModel(VgeDevice &device, const std::vector<Vertex> &vertices) : vgeDevice{device}
	{
		CreateVertexBuffers(vertices);
	}

	VgeModel::~VgeModel()
	{
		vkDestroyBuffer(vgeDevice.device(), vertexBuffer, nullptr);
		vkFreeMemory(vgeDevice.device(), vertexBufferMemory, nullptr);
	}

	void VgeModel::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void VgeModel::Draw(VkCommandBuffer commandBuffer)
	{
		vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}

	void VgeModel::CreateVertexBuffers(const std::vector<Vertex> &vertices)
	{
		vertexCount = static_cast<uint32_t>(vertices.size());
		assert(vertexCount >= 3 && "Vertex count must be at least 3");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		vgeDevice.createBuffer(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBuffer,
			vertexBufferMemory);

		void *data;
		vkMapMemory(vgeDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(vgeDevice.device(), vertexBufferMemory);
	}
}