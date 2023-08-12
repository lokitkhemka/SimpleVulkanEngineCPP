#pragma once

#include "Camera.hpp"
#include "GameObject.hpp"

#include <vulkan/vulkan.h>

namespace vlkn {
	struct FrameInfo {
		int FrameIndex;
		float FrameTime;
		VkCommandBuffer CommandBuffer;
		Camera& camera;
		VkDescriptorSet GlobalDescriptorSet;
		GameObject::Map& GameObjects;
	};
}