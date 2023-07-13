include_directories(
	.
	backends
	../glfw/include
	../glfw/deps
	../glad/include
)

add_library(imgui STATIC
	imgui_draw.cpp
	imgui_tables.cpp
	imgui_widgets.cpp
	imgui.cpp
	backends/imgui_impl_opengl3.cpp
	backends/imgui_impl_glfw.cpp
)