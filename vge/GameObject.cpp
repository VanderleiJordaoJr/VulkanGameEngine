#include "GameObject.h"

namespace vge
{
	glm::mat2 Transform2DComponent::Mat2()
	{
		const float sin = glm::sin(rotation), cos = glm::cos(rotation);

		glm::mat2 rotMatrix = {{cos, sin}, {-sin, cos}};

		glm::mat2 scaleMat{{scale.x, .0f}, {0.f, scale.y}};

		return rotMatrix * scaleMat;
	}

	VgeGameObject VgeGameObject::CreateGameObject()
	{
		static id_t currentId = 0;
		return VgeGameObject{currentId++};
	}

	id_t VgeGameObject::GetId() const
	{
		return id;
	}
}