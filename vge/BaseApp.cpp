#include "BaseApp.h"

#include "Camera.h"
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
		VgeCamera camera{};

		while (!vgeWindow.ShouldClose())
		{
			glfwPollEvents();

			float aspectRatio = vgeRenderer.GetAspectRatio();

			// camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);

			camera.SetPerspectiveProjection(
				glm::radians(50.0f),
				aspectRatio,
				0.1f, 10.0f
			);

			if (auto commandBuffer = vgeRenderer.BeginFrame())
			{
				vgeRenderer.BeginSwapChainRenderPass(commandBuffer);

				renderSystem.RenderGameObjects(commandBuffer, gameObjects, camera);

				vgeRenderer.EndSwapChainRenderPass(commandBuffer);
				vgeRenderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(vgeDevice.device());
	}

	std::unique_ptr<VgeModel> CreateCubeModel(VgeDevice &device, glm::vec3 offset)
	{
		std::vector<VgeModel::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto &v : vertices)
		{
			v.position += offset;
		}
		return std::make_unique<VgeModel>(device, vertices);
	}

	void BaseApp::LoadGameObjects()
	{
		std::shared_ptr<VgeModel> vgeModel = CreateCubeModel(vgeDevice, {.0f, .0f, .0f});
		auto cube = VgeGameObject::CreateGameObject();
		cube.model = vgeModel;

		cube.transform.translation = {.0f, .0f, 2.5f};
		cube.transform.scale = {.5f, .5f, .5f};

		gameObjects.push_back(std::move(cube));
	}
}