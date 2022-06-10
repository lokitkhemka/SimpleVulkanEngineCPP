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
		bool WasWindowResized() { return FramebufferResized; }
		void ResetWindowResizedFlag() { FramebufferResized = false; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	private:
		static void FramebufferResizedCallback(GLFWwindow *window, int Width, int Height);
		void InitWindow();

		int Width;
		int Height;
		bool FramebufferResized;
		std::string WindowName;

		GLFWwindow* WindowHandle;
	};
}