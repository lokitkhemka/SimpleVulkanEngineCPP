#include "Window.hpp"
#include <stdexcept>


vlkn::Window::Window(int w, int h, std::string name):
	Width{ w }, Height{ h }, WindowName{ name }
{
	InitWindow();
}

vlkn::Window::~Window()
{
	glfwDestroyWindow(WindowHandle);
	glfwTerminate();
}

void vlkn::Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, WindowHandle, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to Create Window Surface");
	}
}

void vlkn::Window::FramebufferResizedCallback(GLFWwindow *window, int Width, int Height)
{
	auto WindowRef = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));

	WindowRef->FramebufferResized = true;
	WindowRef->Width = Width;
	WindowRef->Height = Height;
}

void vlkn::Window::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	WindowHandle = glfwCreateWindow(Width, Height, WindowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(WindowHandle, this);
	glfwSetFramebufferSizeCallback(WindowHandle, FramebufferResizedCallback);
}