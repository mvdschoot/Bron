#include "BuiltinShaders.h"

#include "Bron/Core/Core.h"
#include "Bron/Core/Logger.h"

namespace bron {
namespace builtin_shaders {
// The shaders live here, in the translation unit, rather than in files
// next to the executable. Editing one is a recompile, which is the same
// cost as editing the renderer that drives it.
namespace {
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

// The grid has no geometry. This is one screen-filling quad, handed to the GPU already in
// clip space - there is nothing to transform, the quad *is* the screen. Its only job is to
// give every fragment the world-space view ray that goes through it.
layout(location = 0) in vec2 a_ClipPosition;

uniform mat4 uInvViewProjection;

out vec3 v_RayNear; // where this pixel's view ray enters the frustum, in world space
out vec3 v_RayFar; // and where it leaves it

// Clip space -> world space. The perspective divide that the GPU normally does on the way
// out has to be undone by hand on the way back in.
vec3 Unproject(vec3 ndc) {
    vec4 world = uInvViewProjection * vec4(ndc, 1.0);
    return world.xyz / world.w;
}

void main() {
    // z = -1 is the near plane, z = +1 the far plane, so these two points are the ends of
    // the segment of the view ray that is actually inside the frustum. Both lie on a plane
    // of constant depth, which is why interpolating them across the quad stays exact.
    v_RayNear = Unproject(vec3(a_ClipPosition, -1.0));
    v_RayFar = Unproject(vec3(a_ClipPosition, 1.0));

    gl_Position = vec4(a_ClipPosition, 0.0, 1.0);
}

#type fragment
#version 330 core

in vec3 v_RayNear;
in vec3 v_RayFar;

uniform mat4 uViewProjection;
uniform vec3 uCameraPosition;

uniform float uCellSize;
uniform float uPrimaryEvery;
uniform float uFadeStart;
uniform float uFadeEnd;

uniform vec3 uLineColor;
uniform vec3 uPrimaryLineColor;
uniform vec3 uXAxisColor;
uniform vec3 uZAxisColor;
uniform int uDrawAxes;

layout(location = 0) out vec4 o_Color;
// The framebuffer also carries an entity id for mouse picking. The grid covers the whole
// viewport, and an output that is never written leaves its attachment undefined - so where
// the grid does draw it has to stamp "nothing here" explicitly, and where it does not it
// has to discard rather than write, leaving the cleared -1 in place.
layout(location = 1) out int o_EntityId;

// A level of the grid is at full strength once its cells are this many pixels wide, and
// has faded away completely once they have shrunk to the second. Cells below a handful of
// pixels stop reading as lines and start shimmering, so a level is retired well before it
// gets there.
const float kFadeInPixels = 40.0;
const float kFadeOutPixels = 5.0;

// How much of a line pattern of this cell size covers this pixel: 1 on a line, 0 between
// lines, antialiased in between.
//
// fract(coord - 0.5) - 0.5 is the signed distance to the nearest line, in cells. Dividing
// it by fwidth(coord) - how much the coordinate changes from this pixel to the next -
// converts that into a distance in *pixels*. That single division is what makes the grid
// work: lines stay one pixel wide whether the floor is under your nose or at the horizon,
// because the measurement is always made in screen space.
float LineCoverage(vec2 position, float cell_size) {
    vec2 coord = position / cell_size;
    vec2 pixels_to_line = abs(fract(coord - 0.5) - 0.5) / fwidth(coord);

    return 1.0 - min(min(pixels_to_line.x, pixels_to_line.y), 1.0);
}

void main() {
    vec3 origin = v_RayNear;
    vec3 direction = v_RayFar - v_RayNear;

    // Intersect this pixel's view ray with the ground plane y = 0. Solving
    // origin.y + t * direction.y = 0 gives the t below; because the ray was built from the
    // near and far plane, t is inside (0, 1) exactly when the hit is inside the frustum.
    // Written inverted so that a NaN (a ray exactly parallel to the plane) discards too.
    float t = -origin.y / direction.y;
    if (!(t > 0.0 && t < 1.0))
        discard;

    // The piece of floor this pixel is looking at. Everything below is a function of it.
    vec3 world = origin + t * direction;

    // Level of detail. fwidth(world.xz) is how many world units this one pixel covers, so
    // a cell of size s is s / pixel_size pixels wide here. 'level' is how many times
    // uCellSize has to be multiplied by uPrimaryEvery before its cells are kFadeInPixels
    // wide: the finest grid still worth drawing at this distance. Note this is worked out
    // per pixel, not once per frame, so a floor seen at a grazing angle coarsens towards
    // the horizon by itself.
    vec2 world_per_pixel = fwidth(world.xz);
    float pixel_size = max(world_per_pixel.x, world_per_pixel.y);
    float level = max(0.0, log(pixel_size * kFadeInPixels / uCellSize) / log(uPrimaryEvery));

    float fine = uCellSize * pow(uPrimaryEvery, floor(level));
    float mid = fine * uPrimaryEvery;
    float coarse = mid * uPrimaryEvery;

    // How far this level has already handed over to the next one. Driving the hand-over off
    // the cell's size in pixels - rather than off the fractional part of 'level' - is what
    // keeps the finest lines from surviving all the way down to a shimmering one-pixel
    // mush: they are gone once their cells drop below kFadeOutPixels.
    float blend = 1.0 - smoothstep(kFadeOutPixels, kFadeInPixels, fine / pixel_size);

    // Two tiers are on screen at once: ordinary lines, and every uPrimaryEvery-th line
    // drawn brighter. As blend runs 0 -> 1 each tier hands its job over to the next coarser
    // one, so the picture at blend = 1 is exactly the picture at blend = 0 of the next
    // level up - the seam between levels is invisible because there is no seam.
    float lines = mix(LineCoverage(world.xz, fine), LineCoverage(world.xz, mid), blend);
    float primary = mix(LineCoverage(world.xz, mid), LineCoverage(world.xz, coarse), blend);

    vec3 color = mix(uLineColor, uPrimaryLineColor, primary);
    float alpha = max(lines, primary);

    if (uDrawAxes != 0) {
        // The X axis is the line z = 0, the Z axis the line x = 0. Measured in pixels the
        // same way, so they are the same width as every other line.
        vec2 pixels_to_axis = abs(world.xz) / world_per_pixel;
        float on_z_axis = 1.0 - min(pixels_to_axis.x, 1.0);
        float on_x_axis = 1.0 - min(pixels_to_axis.y, 1.0);

        color = mix(color, uZAxisColor, on_z_axis);
        color = mix(color, uXAxisColor, on_x_axis);
        alpha = max(alpha, max(on_x_axis, on_z_axis));
    }

    // Fade with distance instead of ending on a hard edge. The C++ side sizes the fade from
    // the camera's height and keeps it inside the far plane.
    alpha *= 1.0 - smoothstep(uFadeStart, uFadeEnd, length(world - uCameraPosition));
    if (alpha <= 0.0)
        discard;

    o_Color = vec4(color, alpha);

    // The grid is a surface in the world, so it has to depth-test like one. The quad's own
    // depth is meaningless (it sits flat across the screen), so project the point that was
    // actually hit and write that instead: NDC z in [-1, 1], remapped to the [0, 1] the
    // depth buffer stores.
    vec4 clip = uViewProjection * vec4(world, 1.0);
    gl_FragDepth = (clip.z / clip.w) * 0.5 + 0.5;

    o_EntityId = -1;
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
uniform int u_EntityId;

// Array of textures. Texture 0 = pure white texture.
uniform sampler2D u_Textures[32];

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 NormalRaw;
} fs_in;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityId;

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
	EntityId = u_EntityId;
}
)BRON_GLSL";

constexpr const char* Outline3D = R"BRON_GLSL(
#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 3) in vec3 a_SmoothNormal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
// In world units, so the outline gets thinner as the camera pulls away.
uniform float u_OutlineWidth;

void main()
{
	// Pushing every vertex out along its normal grows the mesh by a roughly even margin.
	// It has to be the smoothed normal rather than the shading one: at a hard edge the
	// shading normals disagree, and the corner comes apart into gaps instead of stretching.
	vec3 pos = a_Position + normalize(a_SmoothNormal) * u_OutlineWidth;
	gl_Position = u_Projection * u_View * u_Model * vec4(pos, 1.0);
}


#type fragment
#version 450 core

// In
uniform vec3 u_OutlineColor;
uniform int u_EntityId;

// Out
layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = vec4(u_OutlineColor, 1.0);
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
} // namespace

std::string Source(Id id) {
	switch (id) {
		case Id::kRenderer2D:
			return Renderer2D;
		case Id::kGrid:
			return Grid;
		case Id::kPhong3D:
			return Phong3D;
		case Id::kText:
			return Text;
		case Id::kOutline:
			return Outline3D;
	}

	BR_CORE_ASSERT(false, "Unknown built-in shader");
	return "";
}
} // namespace builtin_shaders
} // namespace bron
