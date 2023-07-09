#type vertex
#version 330 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

out vec2 v_Position;
out vec4 v_Color;
out vec2 v_TexCoord;
flat out float v_TexIndex;

uniform mat4 uVPmatrix;

void main()
{
	v_Position = a_Position;
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	v_TexIndex = a_TexIndex;

	gl_Position = uVPmatrix * vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 330 core

in vec2 v_Position;
in vec4 v_Color;
in vec2 v_TexCoord;
flat in float v_TexIndex;

layout(location = 0) out vec4 FragColor;

uniform sampler2D uTextures[32];

void main()
{
	FragColor = texture(uTextures[int(v_TexIndex)], v_TexCoord) * v_Color;
} 