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

	void BaseApp::LoadGameObjects()
	{
		std::shared_ptr<VgeModel> vgeModel = VgeModel::CreateModelFromFile(vgeDevice, "../models/smooth_vase.obj");
		auto gameObj = VgeGameObject::CreateGameObject();
		gameObj.model = vgeModel;

		gameObj.transform.translation = {.0f, .0f, 2.5f};
		gameObj.transform.scale = glm::vec3(3.f);

		gameObjects.push_back(std::move(gameObj));
	}
}