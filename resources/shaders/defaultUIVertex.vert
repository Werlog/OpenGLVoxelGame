#version 330 core

layout (location = 0) in vec2 screenPos;
layout (location = 1) in vec2 uvCoord;

uniform float aspectRatio;

out vec2 ourTexCoord;

void main()
{
	gl_Position = vec4(screenPos.x * aspectRatio, screenPos.y, 0.0f, 0.0f);

	ourTexCoord = uvCoord;
}