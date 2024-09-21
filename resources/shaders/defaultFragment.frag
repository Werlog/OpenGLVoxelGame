#version 330 core

in vec2 ourTexCoord;
in float lightIntensity;

out vec4 aColor;

uniform sampler2D ourTexture;

void main()
{
	aColor = texture(ourTexture, ourTexCoord) * lightIntensity;
}