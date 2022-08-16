#pragma once

#include "Model.hpp"
#include <memory>

namespace vlkn {

	struct Transform2DComp {
		glm::vec2 Translation{};
		glm::vec2 Scale{1.0f, 1.0f};
		float Rotation;
		glm::mat2 Mat2() {
			const float S = glm::sin(Rotation);
			const float C = glm::cos(Rotation);

			glm::mat2 RotationMat{ {C, S}, {-S, C} };

			glm::mat2 ScaleMat{ {Scale.x, 0.0f}, {0.0f, Scale.y} };
			return RotationMat * ScaleMat; 
		}
	};

	class GameObject {
	public:
		using id_t = unsigned int;
		static GameObject CreateGameObject()
		{
			static id_t CurrentId = 0;
			return GameObject{ CurrentId++ };
		}
		
		//Deleting copy constructor
		GameObject(const GameObject&) = delete;
		//Deleting copy assignment operator
		GameObject& operator=(const GameObject&) = delete;

		//Default Move constructor and Move assignment operator
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;
		
		id_t GetId() { return id; }

		std::shared_ptr<Model> Model{};
		glm::vec3 Color{};
		Transform2DComp Transform2D{};

	private:
		GameObject(id_t ObjId):id{ObjId}{}

		id_t id;
	};
}
