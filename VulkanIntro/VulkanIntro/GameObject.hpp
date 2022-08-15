#pragma once

namespace vlkn {
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
	private:
		GameObject(id_t ObjId):id{ObjId}{}

		id_t id;
	};
}
