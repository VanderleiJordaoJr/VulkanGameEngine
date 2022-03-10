#include "BaseApp.h"

#include "RenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>

namespace vge
{
	BaseApp::BaseApp()
	{
		LoadGameObjects();
	}

	BaseApp::~BaseApp()
	{
	}

	void BaseApp::Run()
	{
		RenderSystem renderSystem{vgeDevice, vgeRenderer.GetSwapChainRenderPass()};

		while (!vgeWindow.ShouldClose())
		{
			glfwPollEvents();

			if (auto commandBuffer = vgeRenderer.BeginFrame())
			{
				vgeRenderer.BeginSwapChainRenderPass(commandBuffer);

				renderSystem.RenderGameObjects(commandBuffer, gameObjects);

				vgeRenderer.EndSwapChainRenderPass(commandBuffer);
				vgeRenderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(vgeDevice.device());
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
		triangle.transform2d.rotation = .025f * glm::two_pi<float>();
		triangle.transform2d.scale = {.5f, .5f};
		triangle.transform2d.translation.x = 0.f;
		gameObjects.push_back(std::move(triangle));
	}
}