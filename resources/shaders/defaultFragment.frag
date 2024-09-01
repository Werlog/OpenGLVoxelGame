#version 330 core

in vec2 ourTexCoord;

out vec4 aColor;

uniform sampler2D ourTexture;

void main()
{
	aColor = texture(ourTexture, ourTexCoord);
}