#pragma once

#include "Model.h"

#include <memory>

namespace vge
{
	struct Transform2DComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{1.f, 1.f};
		float rotation;
		
		glm::mat2 Mat2();
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
		Transform2DComponent transform2d{};

	private:
		VgeGameObject(id_t objId) : id{objId} {};

		id_t id;
	};
}