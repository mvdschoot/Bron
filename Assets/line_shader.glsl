#type vertex
#version 330 core
layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in float a_Width;

out vec4 b_Color;
out float b_Width;

uniform mat4 u_VPmatrix;

void main()
{
	b_Color = a_Color;
	b_Width = a_Width;

	gl_Position = u_VPmatrix * vec4(a_Position, 0.0, 1.0);
}



#type geometry
#version 330 core 
in vec4 b_Color[];
in float b_Width[];

layout (lines) in;
layout (triangle_strip, max_vertices = 6) out;

out vec4 c_Color;
void main() {
	float half_width = b_Width[0] / 2.0;
	vec4 diff = gl_in[1].gl_Position - gl_in[0].gl_Position;
	vec2 rotated = vec2(-diff[1], diff[0]);
	vec2 w = normalize(rotated) * half_width;  

	// calc Vertices
	vec4 ldown = vec4(gl_in[0].gl_Position[0] + w[0], gl_in[0].gl_Position[1] + w[1], gl_in[0].gl_Position[2], gl_in[0].gl_Position[3]);
	vec4 rdown = vec4(gl_in[0].gl_Position[0] - w[0], gl_in[0].gl_Position[1] - w[1], gl_in[0].gl_Position[2], gl_in[0].gl_Position[3]);
	vec4 lup = ldown + diff;
	vec4 rup = rdown + diff;

	// Emit vertices
	c_Color = b_Color[0];

	gl_Position = ldown;
	EmitVertex();

	gl_Position = rdown;
	EmitVertex();

	gl_Position = lup;
	EmitVertex();
	EndPrimitive();

	gl_Position = lup;
	EmitVertex();

	gl_Position = rup;
	EmitVertex();

	gl_Position = rdown;
	EmitVertex();
	EndPrimitive();

}




#type fragment
#version 330 core

in vec4 c_Color;

out vec4 FragColor;

void main()
{
	FragColor = c_Color;
}