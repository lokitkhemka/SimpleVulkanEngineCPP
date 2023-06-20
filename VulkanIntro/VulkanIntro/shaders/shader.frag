#version 450
layout(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 OutColor;

layout(push_constant) uniform Push{
	mat4 transform;
	mat4 modelMatrix;
} push;

void main()
{
	OutColor = vec4(fragColor,1.0);
}