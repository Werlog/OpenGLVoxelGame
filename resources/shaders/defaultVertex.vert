#version 330 core

layout (location = 0) in int data;
layout (location = 1) in vec2 uvData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDirection;

out vec2 ourTexCoord;
out float lightIntensity;

vec3 voxelNormals[6] = vec3[6](
	vec3(0, 0, 1),
	vec3(1, 0, 0),
	vec3(0, 0, -1),
	vec3(-1, 0, 0),
	vec3(0, 1, 0),
	vec3(0, -1, 0)
);

void main()
{
	int positionX = data & 31;
	int positionY = (data >> 5) & 511;
	int positionZ = (data >> 14) & 31;
	int faceIndex = (data >> 19) & 7;

	vec3 normal = voxelNormals[faceIndex];
	
	lightIntensity = max(dot(normal, lightDirection), 0.2f);
	lightIntensity += 0.5f; // Ambient
	

	gl_Position = projection * view * model * vec4(positionX, positionY, positionZ, 1);
	ourTexCoord = uvData;
}