#version 450

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;


layout(location=0) out vec3 fragColor;

layout(set=0, binding=0) uniform GlobalUBO{
	mat4 projectionViewMatrix;
	vec4 AmbientLightColor;
	vec3 LightPosition;
	vec4 LightColor;
} ubo;

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;


void main()
{
	vec4 VertexPosition_World = push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projectionViewMatrix * VertexPosition_World;

	vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

	vec3 DirectionToLight = ubo.LightPosition - VertexPosition_World.xyz;
	float Attenuation = 1.0/ dot(DirectionToLight, DirectionToLight);

	vec3 LightColor = ubo.LightColor.xyz * ubo.LightColor.w * Attenuation;
	vec3 AmbientLight = ubo.AmbientLightColor.xyz * ubo.AmbientLightColor.w;
	vec3 DiffuseLight = LightColor * max(dot(normalWorldSpace, normalize(DirectionToLight)),0);

	
	fragColor = (DiffuseLight + AmbientLight) * color;
}