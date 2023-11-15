/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2023 Yuriy Zinchenko

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

#include <cglm/affine.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <SDL_events.h>
#include "Common/Material.h"
#include "Core/Global.h"
#include "Core/ILog.h"
#include "Input/SystemEvent.h"
#include "Resource/Model.h"
#include "Resource/Shader.h"
#include "Resource/Material.h"
#include "System/Renderer.h"
#include "System/Window.h"
#include "World/VisualWorld.h"

using namespace A3D;

static void OnQuit(const SDL_Event& event, void* run)
{
	*reinterpret_cast<bool*>(run) = false;
}

/*static void LoadModel(MeshGroup& mesh, const char* filename)
{
	MeshFileReader file;
	if (!OpenMeshFile(file, filename))
	{
		LogFatal("Could not load mesh file \"%s\": file not found.", filename);
		return;
	}

	Box box;
	Sphere sphere;
	MeshFileChunk chunk = MeshFileChunk::END_OF_FILE;
	do
	{
		chunk = GetNextMeshFileChunk(file);
		switch (chunk)
		{
		case MeshFileChunk::VERTEX_BUFFER:
			ReadMeshVertexBuffer(file, mesh, box, sphere, false);
			break;
		case MeshFileChunk::VERTEX_BUFFER_COMPRESSED:
			ReadMeshVertexBuffer(file, mesh, box, sphere, true);
			break;
		case MeshFileChunk::INDEX_BUFFER:
			ReadMeshIndexBuffer(file, mesh, false);
			break;
		case MeshFileChunk::INDEX_BUFFER_COMPRESSED:
			ReadMeshIndexBuffer(file, mesh, true);
			break;
		default:
			;
		}
	}
	while (chunk != MeshFileChunk::END_OF_FILE);
}*/

int main()
{
	bool run = true;

	SystemEventListener listener;
	CreateSystemEventListener();
	BindSystemEvent(listener, SDL_QUIT, OnQuit, &run);

	CreateVideo();

	WindowResolution wres{800, 600};
	CreateWindow("Hello, World!", wres, WindowMode::WINDOWED, false);

	WindowPlatformData wpd;
	GetWindowPlatformData(wpd);

	RendererGpu gpu{};
	RendererResolution rres{wres.width, wres.height};
//	CreateRenderer(gpu, wpd.window, wpd.display, rres, RendererType::OpenGL, 16, 0, false, false);
	CreateRenderer(gpu, wpd.window, wpd.display, rres, RendererType::Auto, 16, 0, false, false);

	const uint8_t threads_count = 2;
	RendererThreadContext* contexts = CreateRendererThreadContexts(threads_count);

	VisualWorld vw;
	CreateVisualWorld(vw);

	vec3 camera_pos {0.0f, 0.0f, 400.0f};
	vec3 camera_target {0.0f, 0.0f, 0.0f};
	SetCameraPerspective(vw, MAIN_VIEWPORT, glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 512.0f);
	SetCameraLookAt(vw, MAIN_VIEWPORT, camera_pos, camera_target);

	Model model;
	GetModel(model, "../Data/Models/Box.mdl");

	GlobalTransform transform{ GLM_MAT4_IDENTITY };

	static constexpr const char* MATERIAL_NAMES[] =
	{
		"../Data/Materials/WhiteUnlit.xml",
		"../Data/Materials/RedUnlit.xml",
		"../Data/Materials/GreenUnlit.xml",
		"../Data/Materials/BlueUnlit.xml"
	};

	VisualHandle cubes[4096];
	Material material;
	vec3 pos{};
	for (int i = 0; i < 4096; ++i)
	{
		if (!GetMaterial(material, MATERIAL_NAMES[i % 4]))
			return 1;

		glm_mat4_identity(transform.transform);
		pos[0] = (float)(i % 64 * 4 - 128);
		pos[1] = (float)(i / 64 * 4 - 128);
		glm_translate(transform.transform, pos);
		cubes[i] = AddModel(vw, model.groups[0], material, transform);
	}

	vec3 rotate_axis[5] =
	{
		{ 0.0f, 0.0f, 1.0f },
		{ 1.0f, 0.5f, 0.0f },
		{ 0.0f, 1.0f, 0.5f },
		{ 0.5f, 0.0f, 1.0f },
		{ 0.5f, 1.0f, 0.0f }
	};

	while (run)
	{
		PollSystemEvents(listener);

		for (unsigned i = 0; i < 4096; ++i)
			glm_rotate(GetVisualObjectTransform(vw, cubes[i]).transform, glm_rad(0.1f), rotate_axis[i % 5]);

		PrepareVisualWorld(vw);

		RenderVisualWorld(vw, contexts, threads_count);

		UpdateShaderCache();
	}

	for (int i = 0; i < 4096; ++i)
		ReleaseMaterial(material);
	ReleaseModel("../Data/Models/Box.mdl");
	DestroyVisualWorld(vw);

	DestroyRendererThreadContexts(contexts);
	DestroyRenderer();
	DestroyWindow();
	DestroyVideo();
	DestroySystemEventListener();

	return 0;
}
