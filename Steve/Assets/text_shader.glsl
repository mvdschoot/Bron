#type vertex
#version 330 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;

out vec4 f_Color;
out vec2 f_TexCoord;
out float f_TexIndex;

uniform mat4 uVPmatrix;

void main()
{
	gl_Position = uVPmatrix * vec4(a_Position.xy, 0.0, 1.0);

    f_Color = a_Color;
    f_TexCoord = a_TexCoord;
    f_TexIndex = a_TexIndex;
}  

#type fragment
#version 330 core
in vec4 f_Color;
in vec2 f_TexCoord;
in float f_TexIndex;

out vec4 color;

uniform sampler2D uTextures[32];

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uTextures[int(f_TexIndex)], f_TexCoord).r);
    color = f_Color * sampled;
}  