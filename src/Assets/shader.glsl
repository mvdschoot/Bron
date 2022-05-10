#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_uv;

out vec2 v_uv;

uniform mat4 a_VPmatrix;

void main()
{
	v_uv = a_uv;
	gl_Position = a_VPmatrix * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 FragColor;

uniform sampler2D text;

in vec2 v_uv;

void main()
{
	FragColor = texture(text, v_uv);
} 