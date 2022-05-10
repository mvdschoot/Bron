#version 330 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D text;

in vec2 v_uv;

void main()
{
	FragColor = texture(text, v_uv);
} 