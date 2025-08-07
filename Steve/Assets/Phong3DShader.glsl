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
