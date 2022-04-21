/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022 Yuriy Zinchenko

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include "Core/Coordinator.h"

struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
};

static constexpr PosColorVertex VERTICES[] = {
	{-1.0f, 1.0f, 1.0f, 0xff000000},
	{1.0f, 1.0f, 1.0f, 0xff0000ff},
	{-1.0f, -1.0f, 1.0f, 0xff00ff00},
	{1.0f, -1.0f, 1.0f, 0xff00ffff},
	{-1.0f, 1.0f, -1.0f, 0xffff0000},
	{1.0f, 1.0f, -1.0f, 0xffff00ff},
	{-1.0f, -1.0f, -1.0f, 0xffffff00},
	{1.0f, -1.0f, -1.0f, 0xffffffff},
};

static constexpr uint16_t INDICES[] = {
	0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6, 1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

int main()
{
	Coordinator::InitInfo initInfo;
	initInfo.graphics.title = "Hello World";
	initInfo.graphics.width = 800;
	initInfo.graphics.height = 600;
	initInfo.graphics.vsync = true;

	bool initialized;
	Coordinator coordinator(initInfo, initialized);
	if (!initialized)
		return 1;

	Graphics& graphics = coordinator.GetGraphics();
	bgfx::VertexBufferHandle vbo = graphics.CreateVertexBuffer(VERTICES, sizeof(VERTICES));
	bgfx::IndexBufferHandle ebo = graphics.CreateIndexBuffer(INDICES, sizeof(INDICES));
	bgfx::ShaderHandle vertex = graphics.LoadShader("01_hello_world.vert.shader");
	bgfx::ShaderHandle fragment = graphics.LoadShader("01_hello_world.frag.shader");
	bgfx::ProgramHandle program = graphics.CreateProgram(vertex, fragment);

	while (coordinator.IsRunning())
	{
		coordinator.BeginFrame();

		glm::vec3 eye(0.0f, 0.0f, -5.0f);
		glm::vec3 center(0.0f, 0.0f, 0.0f);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::mat view = glm::lookAt(eye, center, up);
		graphics.SetViewMatrix(view);

		const float ticks = static_cast<float>(coordinator.GetTicks());
		glm::vec3 rot(ticks * 0.001f, 0.0f, 0.0f);
		glm::quat quat(rot);
		glm::mat model = glm::toMat4(quat);
		graphics.SetTransform(model);

		graphics.Draw(program, vbo, ebo);

		coordinator.EndFrame();
	}

	return coordinator.Run();
}
