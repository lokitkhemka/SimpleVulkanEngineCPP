#pragma once

#include "Model.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace vlkn {
	//Model Transform
	struct TransformComponent {
		glm::vec3 Translation{};
		glm::vec3 Scale{1.0f, 1.0f,1.0f};
		glm::vec3 Rotation{};
		glm::mat4 Mat4() {
			auto Transform = glm::translate(glm::mat4(1.0f), Translation);
			Transform = glm::rotate(Transform, Rotation.y, {0.0f,1.0f,0.0f});
			Transform = glm::rotate(Transform, Rotation.x, { 1.0f,0.0f,0.0f });
			Transform = glm::rotate(Transform, Rotation.z, { 0.0f,0.0f,1.0f });
			Transform = glm::scale(Transform, Scale);
			return Transform;
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
		TransformComponent Transform{};

	private:
		GameObject(id_t ObjId):id{ObjId}{}

		id_t id;
	};
}
