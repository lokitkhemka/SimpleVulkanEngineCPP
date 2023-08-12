#version 450

layout(location = 0) in vec3 position; 
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;


layout(set=0, binding=0) uniform GlobalUBO{
	mat4 projectionViewMatrix;
	vec4 AmbientLightColor;
	vec3 LightPosition;
	vec4 LightColor;
} ubo;

layout(location=0) out vec3 fragColor;
layout(location=1) out vec3 fragPosWorld;
layout(location=2) out vec3 fragNormalWorld;



layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;


void main()
{
	vec4 VertexPosition_World = push.modelMatrix * vec4(position, 1.0);
	gl_Position = ubo.projectionViewMatrix * VertexPosition_World;

	fragNormalWorld = normalize(mat3(push.normalMatrix) * normal);
	fragPosWorld = VertexPosition_World.xyz;
	fragColor = color;
}