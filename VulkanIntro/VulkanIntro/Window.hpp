#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vlkn {
	class Window {
	public:
		Window(int w, int h, std::string name);
		~Window();

		//Prevent from creating a copy of a Window object
		Window(const Window&) = delete;
		Window& operator=(const Window& Window)=delete;
		bool ShouldClose() { return glfwWindowShouldClose(WindowHandle); }

		VkExtent2D getExtent() { return { static_cast<uint32_t>(Width), static_cast<uint32_t>(Height)	 }; }
		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		void InitWindow();

		const int Width;
		const int Height;
		std::string WindowName;

		GLFWwindow* WindowHandle;
	};
}