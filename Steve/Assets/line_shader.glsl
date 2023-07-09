#type vertex
#version 330 core
layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 3) in float a_Width;

out vec2 TexCoords;

uniform mat4 uVPmatrix;

void main()
{
    gl_Position = uVPmatrix * vec4(a_Position, 0.0, 1.0);
    TexCoords = vertex.zw;
}  

#type fragment
#version 330 core
layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 3) in float a_Width;


void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    color = vec4(textColor, 1.0) * sampled;
}  