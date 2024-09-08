#version 330 core

layout (location = 0) in int data;
layout (location = 1) in vec2 uvData;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 ourTexCoord;

void main()
{
	int positionX = data & 31;
	int positionY = (data >> 5) & 511;
	int positionZ = (data >> 14) & 31;

	gl_Position = projection * view * model * vec4(positionX, positionY, positionZ, 1);
	ourTexCoord = uvData;
}