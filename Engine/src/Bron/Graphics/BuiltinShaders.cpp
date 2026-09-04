#include "BuiltinShaders.h"

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace bron
{
	namespace builtin_shaders
	{
		// The shaders live here, in the translation unit, rather than in files
		// next to the executable. Editing one is a recompile, which is the same
		// cost as editing the renderer that drives it.
		namespace
		{
		constexpr const char* Renderer2D = R"BRON_GLSL(
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
)BRON_GLSL";

		constexpr const char* Grid = R"BRON_GLSL(
#type vertex
#version 330 core
layout(location = 0) in float a_Position; //Dummy

out vec3 nearPoint;
out vec3 farPoint;

uniform mat4 uView;
uniform mat4 uProjection;
// uniform mat4 uPosition;

// Grid position are in clipped space
vec3 gridPlane[6] = vec3[] (
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = gridPlane[gl_VertexID].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, uView, uProjection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, uView, uProjection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}

#type fragment
#version 330 core
in vec3 nearPoint; // nearPoint calculated in vertex shader
in vec3 farPoint; // farPoint calculated in vertex shader

uniform mat4 uView;
uniform mat4 uProjection;

out vec4 outColor;

const float near = 0.1;
const float far = 100.0;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}
float computeDepthRaw(vec3 pos) {
    vec4 clip_space_pos = uProjection * uView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}
float computeDepth(vec3 pos) {
    return ((gl_DepthRange.diff * computeDepthRaw(pos)) +
                gl_DepthRange.near + gl_DepthRange.far) / 2.0;
}

float computeLinearDepth(vec3 pos) {
    float clip_space_depth = computeDepth(pos);
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}
void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    outColor = (grid(fragPos3D, 10, true) + grid(fragPos3D, 1, true))* float(t > 0); // adding multiple resolution for the grid
    outColor.a *= fading;
}
)BRON_GLSL";

		constexpr const char* Phong3D = R"BRON_GLSL(
#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

out VS_OUT {
	vec3 FragPos;	    // world-space position
	vec3 Normal;		// world-space normal
	vec2 TexCoords;
	vec3 NormalRaw;
} vs_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
	vec4 worldPos = u_Model * vec4(a_Position, 1.0);
	vs_out.FragPos = worldPos.xyz;

	// Transform normal to world space (ignoring non-uniform scaling correction)
	vs_out.Normal = mat3(transpose(inverse(u_Model))) * a_Normal;

	vs_out.TexCoords = a_TexCoords;

	vs_out.NormalRaw = a_Normal;

	gl_Position = u_Projection * u_View * worldPos;
}



#type fragment
#version 450 core

struct Material {
	float AmbientFactor;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
	float ShininessStrength;
	float DiffuseTexture; // index in the texture array
	float SpecularTexture; // index in the texture array
};

struct LightStruct {
	vec3 Position;  // world-space
	vec3 Color;
};

layout(std140, binding = 0) uniform PointLightBlock {
	LightStruct POINTLIGHTS[32];
};

uniform int u_NumPointLights;
uniform vec3 u_ViewPos;
uniform Material u_Material;

// Array of textures. Texture 0 = pure white texture.
uniform sampler2D u_Textures[32];

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 NormalRaw;
} fs_in;

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(fs_in.Normal);
	vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);

	// Base color from material and texture
	int diffuseIdx = int(u_Material.DiffuseTexture);
	int specularIdx = int(u_Material.SpecularTexture);

	vec3 texDiffuse = texture(u_Textures[diffuseIdx], fs_in.TexCoords).rgb;
	vec3 texSpecular = texture(u_Textures[specularIdx], fs_in.TexCoords).rgb;

	vec3 ambient = u_Material.AmbientFactor * u_Material.Diffuse * texDiffuse;

	vec3 result = ambient;

	for(int i = 0; i < u_NumPointLights; ++i)
	{
		vec3 lightDir = normalize(POINTLIGHTS[i].Position - fs_in.FragPos);

		// Diffuse shading
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * u_Material.Diffuse * texDiffuse * POINTLIGHTS[i].Color;

		// Specular shading (Phong)
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.Shininess);
		vec3 specular = spec * u_Material.Specular * texSpecular * POINTLIGHTS[i].Color * u_Material.ShininessStrength;

		result += diffuse + specular;
	}

	FragColor = vec4(result, 1.0);
	// FragColor = vec4(fs_in.NormalRaw * 0.5 + 0.5, 1.0);
}
)BRON_GLSL";

		constexpr const char* Text = R"BRON_GLSL(
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
)BRON_GLSL";
		}

		std::string Source(Id id)
		{
			switch (id)
			{
			case Id::kRenderer2D: return Renderer2D;
			case Id::kGrid:       return Grid;
			case Id::kPhong3D:    return Phong3D;
			case Id::kText:       return Text;
			}

			BR_CORE_ASSERT(false, "Unknown built-in shader");
			return "";
		}
	}
}
