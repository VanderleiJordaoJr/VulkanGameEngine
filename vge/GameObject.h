#pragma once

#include "Model.h"

#include <memory>

namespace vge
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{1.f, 1.f, 1.f};
		glm::vec3 rotation;
		
		glm::mat4 Mat4();
	};

	class VgeGameObject
	{
	public:
		using id_t = unsigned int;

		static VgeGameObject CreateGameObject();

		VgeGameObject(const VgeGameObject &) = delete;
		VgeGameObject &operator=(const VgeGameObject &) = delete;
		VgeGameObject(VgeGameObject &&) = default;
		VgeGameObject &operator=(VgeGameObject &&) = default;

		id_t GetId() const;

		std::shared_ptr<VgeModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		VgeGameObject(id_t objId) : id{objId} {};

		id_t id;
	};
}