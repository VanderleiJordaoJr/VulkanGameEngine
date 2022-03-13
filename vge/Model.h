#pragma once

#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace vge
{
	class VgeModel
	{
	public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();

			bool operator==(const Vertex &other) const;
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void LoadModel(const std::string &filepath);
		};

		VgeModel(VgeDevice &device, const VgeModel::Builder &builder);
		~VgeModel();
		VgeModel(const VgeModel &) = delete;
		VgeModel &operator=(const VgeModel &) = delete;

		static std::unique_ptr<VgeModel> CreateModelFromFile(VgeDevice &device, const std::string &filepath);

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

	private:
		void CreateVertexBuffers(const std::vector<Vertex> &vertices);
		void CreateIndexBuffers(const std::vector<uint32_t> &indices);

		VgeDevice &vgeDevice;

		bool hasIndexBuffer = false;

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		uint32_t vertexCount;

		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		uint32_t indexCount;
	};
}