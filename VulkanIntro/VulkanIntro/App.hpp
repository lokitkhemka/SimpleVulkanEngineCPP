#pragma once

#include "Window.hpp"
#include "Pipeline.hpp"
#include "VulkanDevice.hpp"
#include "Swapchain.hpp"
#include "Model.hpp"

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
		void LoadModels();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void DrawFrame();


		Window window{WIDTH, HEIGHT, "Vulkan Window"};
		VulkanDevice Device{ window };
		Swapchain swapchain{ Device, window.getExtent() };
		std::unique_ptr<Pipeline> pipeline;
		VkPipelineLayout PipelineLayout;
		std::vector<VkCommandBuffer> CommandBuffers;
		std::unique_ptr<Model> model;


		void sierpinski(
			std::vector<Model::Vertex>& vertices,
			int depth,
			glm::vec2 left,
			glm::vec2 right,
			glm::vec2 top);

	};
}