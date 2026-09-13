#include "Grid.h"

#include "Command.h"
#include "Bron/Graphics/BuiltinShaders.h"

#include "../Buffer.h"
#include "../Shader.h"
#include "../VertexArray.h"

#include <algorithm>
#include <cmath>

namespace bron {
namespace {
struct GridData {
	Ref<VertexArray> vao;
	Ref<VertexBuffer> vbo;
	Ref<IndexBuffer> ibo;

	Ref<Shader> shader;
	Camera* camera = nullptr;

	GridSettings settings;
};

GridData g_data;

// The quad is given straight in clip space: it is not a thing in the world that needs
// transforming, it is just "every pixel of the screen". Two triangles, corners at the
// edges of the viewport.
constexpr float kClipSpaceQuad[8] = {
		-1.0f, -1.0f, // bottom left
		1.0f,  -1.0f, // bottom right
		1.0f,  1.0f, // top right
		-1.0f, 1.0f, // top left
};
constexpr u32 kQuadIndices[6] = {0, 1, 2, 2, 3, 0};

// The camera interface does not carry its clip planes, but a perspective projection does:
// glm::perspective stores -(f+n)/(f-n) at [2][2] and -2fn/(f-n) at [3][2], and those two
// invert to the distance below. The grid needs it so its fade can finish just before the
// far plane - past that the floor is clipped away, and a fade that has not finished yet
// ends on a hard circle.
float FarPlaneOf(const glm::mat4& projection) {
	const float denominator = projection[2][2] + 1.0f;
	if (std::abs(denominator) < 1e-6f) // Orthographic, or something else without a usable far plane.
		return 1000.0f;

	return projection[3][2] / denominator;
}
} // namespace

void GridRenderer::Init(Camera* camera) {
	BR_PROFILE_FUNCTION();

	g_data.camera = camera;
	g_data.shader = Shader::Create(builtin_shaders::Source(builtin_shaders::Id::kGrid));

	float vertices[8];
	std::copy(std::begin(kClipSpaceQuad), std::end(kClipSpaceQuad), std::begin(vertices));
	g_data.vbo = VertexBuffer::Create(vertices, sizeof(vertices));
	g_data.vbo->SetBufferLayout({{"a_ClipPosition", ShaderDataType::kFloat2}});

	u32 indices[6];
	std::copy(std::begin(kQuadIndices), std::end(kQuadIndices), std::begin(indices));
	g_data.ibo = IndexBuffer::Create(indices, 6);

	g_data.vao = VertexArray::Create();
	g_data.vao->Bind();
	g_data.vao->SetIndexBuffer(g_data.ibo);
	g_data.vao->AddVertexBuffer(g_data.vbo);
}

GridSettings& GridRenderer::Settings() { return g_data.settings; }

void GridRenderer::Draw() {
	BR_PROFILE_FUNCTION();
	BR_CORE_ASSERT(g_data.camera, "GridRenderer::Draw before Init");

	const GridSettings& settings = g_data.settings;

	const glm::mat4 projection = g_data.camera->GetProjectionMatrix();
	const glm::mat4 view_projection = projection * g_data.camera->GetViewMatrix();
	const glm::vec3 camera_position = g_data.camera->GetPosition();

	// How far the grid should reach. Tying it to the camera's height above the plane keeps
	// the amount of grid on screen roughly constant while you zoom, and the far plane is a
	// hard ceiling: anything past it is clipped, not faded.
	const float height = glm::max(std::abs(camera_position.y), settings.cell_size);
	const float fade_end = glm::min(height * settings.view_distance, FarPlaneOf(projection) * 0.95f);
	const float fade_start = fade_end * 0.55f;

	g_data.shader->Bind();

	// The vertex stage turns pixels into world-space rays, so it needs the inverse; the
	// fragment stage projects the point it hit back to a depth, so it needs the forward.
	g_data.shader->SetUniformMat4("uViewProjection", view_projection);
	g_data.shader->SetUniformMat4("uInvViewProjection", glm::inverse(view_projection));
	g_data.shader->SetUniform3f("uCameraPosition", camera_position.x, camera_position.y, camera_position.z);

	g_data.shader->SetUniform1f("uCellSize", glm::max(settings.cell_size, 1e-3f));
	// The shader takes a logarithm to this base to pick its level of detail, so anything
	// at or below 1 would mean "never coarsen" - or a division by zero.
	g_data.shader->SetUniform1f("uPrimaryEvery", glm::max(settings.primary_every, 2.0f));
	g_data.shader->SetUniform1f("uFadeStart", fade_start);
	g_data.shader->SetUniform1f("uFadeEnd", fade_end);

	g_data.shader->SetUniform3f("uLineColor", settings.line_color.r, settings.line_color.g, settings.line_color.b);
	g_data.shader->SetUniform3f("uPrimaryLineColor", settings.primary_line_color.r, settings.primary_line_color.g,
								settings.primary_line_color.b);
	g_data.shader->SetUniform3f("uXAxisColor", settings.x_axis_color.r, settings.x_axis_color.g,
								settings.x_axis_color.b);
	g_data.shader->SetUniform3f("uZAxisColor", settings.z_axis_color.r, settings.z_axis_color.g,
								settings.z_axis_color.b);
	g_data.shader->SetUniform1i("uDrawAxes", settings.draw_axes ? 1 : 0);

	Command::DrawIndexed(g_data.vao, 6);
}
} // namespace bron
