#pragma once

#include "Window.hpp"
#include "VulkanDevice.hpp"
#include "Renderer.hpp"
#include "GameObject.hpp"
#include "VulkanDescriptors.hpp"

#include <memory>
#include <vector>

namespace vlkn {
	class App {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		App();
		~App();

		App(const App &) = delete;
		App& operator=(const App&) = delete;
		void run();
	private:
		void LoadGameObjects();


		Window window{WIDTH, HEIGHT, "Vulkan Window"};
		VulkanDevice Device{ window };
		Renderer renderer{ window, Device };

		std::unique_ptr<VulkanDescriptorPool> GlobalPool{};
		GameObject::Map GameObjects;


	};
}