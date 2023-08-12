#version 450
layout(location = 0) in vec3 fragColor;
layout(location=1) in vec3 fragPosWorld;
layout(location=2) in vec3 fragNormalWorld;

layout(location = 0) out vec4 OutColor;

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
	vec3 DirectionToLight = ubo.LightPosition - fragPosWorld;
	float Attenuation = 1.0/ dot(DirectionToLight, DirectionToLight);

	vec3 LightColor = ubo.LightColor.xyz * ubo.LightColor.w * Attenuation;
	vec3 AmbientLight = ubo.AmbientLightColor.xyz * ubo.AmbientLightColor.w;
	vec3 DiffuseLight = LightColor * max(dot(normalize(fragNormalWorld), normalize(DirectionToLight)),0);

	
	OutColor = vec4((DiffuseLight + AmbientLight) * fragColor,1.0);
}