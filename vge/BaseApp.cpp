#include "BaseApp.h"

#include "Camera.h"
#include "Keyboard.h"
#include "RenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono>
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

		// camera.SetViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
		camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		auto viewerObject = VgeGameObject::CreateGameObject();
		VgeKeyboard keyboard{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!vgeWindow.ShouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();

			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

			currentTime = newTime;

			if (glfwGetKey(vgeWindow.GetGLFWWindow(), keyboard.keys.quit) == GLFW_PRESS)
				break;

			keyboard.MoveInPlaneXZ(vgeWindow.GetGLFWWindow(), frameTime, viewerObject);

			camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspectRatio = vgeRenderer.GetAspectRatio();

			// camera.SetOrthographicProjection(-aspectRatio, aspectRatio, -1, 1, -1, 1);

			camera.SetPerspectiveProjection(
				glm::radians(50.0f),
				aspectRatio,
				0.1f, 10.0f);

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
		VgeModel::Builder modelBuilder{};
		modelBuilder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto &v : modelBuilder.vertices)
		{
			v.position += offset;
		}

		modelBuilder.indices = {0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

		return std::make_unique<VgeModel>(device, modelBuilder);
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