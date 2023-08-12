#pragma once

#include "Model.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace vlkn {
	//Model Transform
	struct TransformComponent {
		glm::vec3 Translation{};
		glm::vec3 Scale{1.0f, 1.0f,1.0f};
		glm::vec3 Rotation{};
		glm::mat4 Mat4();
		glm::mat3 NormalMatrix();
	};


	class GameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, GameObject>;
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
