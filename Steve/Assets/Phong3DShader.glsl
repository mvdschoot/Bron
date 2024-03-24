#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
//layout(location = 3) in float a_MaterialIndex;

out vec3 f_Position;
out vec3 f_Normal;
out vec2 f_TexCoords;
//flat out float f_MaterialIndex;

uniform mat4 uVPmatrix;
uniform mat4 uTransform;

void main() {
	f_Position = (uTransform * vec4(a_Position, 1)).xyz;
    f_Normal = normalize(mat3(transpose(inverse(mat4(1.0)))) * a_Normal);
    f_TexCoords = a_TexCoords;
	//f_MaterialIndex = a_MaterialIndex;
    
	gl_Position = uVPmatrix * (uTransform * vec4(a_Position, 1.0));
}


#type fragment
#version 330 core

struct Material {
	float AmbientFactor;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
	float ShininessStrength;
	float DiffuseTexture;
	float SpecularTexture;
	float NormalTexture;
};

struct LightStruct {
	vec3 Position;
	vec3 Color;
};

in vec3 f_Position;
in vec3 f_Normal;
in vec2 f_TexCoords;
//flat in float f_MaterialIndex;

layout(location = 0) out vec4 FragColor;

uniform Material uMaterial;
uniform LightStruct uLight[32];
uniform sampler2D uTextures[32];

uniform mat4 uViewMatrix;


void main()
{
	vec3 norm;
	if (uMaterial.NormalTexture == 0.0) {
		norm = normalize(f_Normal);
	} else {
		norm = normalize(texture(uTextures[int(uMaterial.NormalTexture)], f_TexCoords).rgb);
	}

	vec3 viewDir = normalize(vec3(inverse(uViewMatrix) * vec4(0.0,0.0,0.0,1.0)) - f_Position);

	// Diffuse lighting
	// Lot of duplicate code, because now only 1 if-statement
	vec4 diffuse = vec4(0,0,0,1);
	vec4 initDiffuseColor;
	if (uMaterial.DiffuseTexture == 0.0) {
		initDiffuseColor = vec4(uMaterial.Diffuse, 1.0);
	} else {
		initDiffuseColor = texture(uTextures[int(uMaterial.DiffuseTexture)], f_TexCoords);
	}

	for(int i = 0; i < 32; i++) {
		vec3 lightDir = normalize(uLight[i].Position - f_Position);
		float diff = max(dot(norm, lightDir), 0.0);
		diffuse += vec4(uLight[i].Color, 1.0) * initDiffuseColor * diff;
	}

	// Specular lighting 
	// Lot of duplicate code, because now only 1 if-statement
	vec4 initSpecularColor;
	if (uMaterial.SpecularTexture == 0.0) {
		initSpecularColor = vec4(uMaterial.Specular, 1.0);
	} else {
		initSpecularColor = texture(uTextures[int(uMaterial.SpecularTexture)], f_TexCoords);
	}
	
	vec4 specular = vec4(0,0,0,1);
	for(int i = 0; i < 32; i++) {
		vec3 lightDir = normalize(uLight[i].Position - f_Position);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.Shininess);
		specular += vec4(uLight[i].Color, 1.0) * initSpecularColor * spec * uMaterial.ShininessStrength;
	}
	 
	// Combine all the lighting components
	FragColor = uMaterial.AmbientFactor * initDiffuseColor + (diffuse+ specular);
}