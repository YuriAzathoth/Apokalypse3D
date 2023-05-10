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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <cglm/cglm.h>
#include <doctest/doctest.h>
#include "Scene/SceneGraph.h"

TEST_SUITE("Scene Graph")
{
	TEST_CASE("Idle")
	{
		A3D::SceneGraph sg;
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 0);
	}

	TEST_CASE("Add 1")
	{
		A3D::SceneGraph sg;
		const A3D::NodeHandle node = sg.AddNode();
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.IsNodeExists(node));
		REQUIRE(!sg.IsValid(sg.GetParent(node)));
		REQUIRE(!sg.IsValid(sg.GetFirstChild(node)));
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(node)));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(node)));
	}

	TEST_CASE("Remove 1")
	{
		A3D::SceneGraph sg;
		const A3D::NodeHandle node = sg.AddNode();
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.IsNodeExists(node));

		sg.RemoveNode(node);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 0);
		REQUIRE(!sg.IsNodeExists(node));
	}

	TEST_CASE("Add horizontal 2")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[2];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 2);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsValid(sg.GetParent(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetParent(nodes[1])));
		REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[1])));
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[1])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[1])));
	}

	TEST_CASE("Add horizontal 5")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(sg.IsNodeExists(nodes[i]));
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
	}

	TEST_CASE("Add horizontal 10")
	{
		constexpr unsigned SIZE = 10;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(sg.IsNodeExists(nodes[i]));
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
	}

	TEST_CASE("Add horizontal 50")
	{
		constexpr unsigned SIZE = 50;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(sg.IsNodeExists(nodes[i]));
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
	}

	TEST_CASE("Add vertical 2")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[2];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 1);

		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));

		REQUIRE(!sg.IsValid(sg.GetParent(nodes[0])));
		REQUIRE(sg.GetFirstChild(nodes[0]).handle == nodes[1].handle);
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[0])));

		REQUIRE(sg.GetParent(nodes[1]).handle == nodes[0].handle);
		REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[1])));
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[1])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[1])));
	}

	TEST_CASE("Add vertical 5")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		nodes[0] = sg.AddNode();
		for (unsigned i = 1; i < SIZE; ++i)
			nodes[i] = sg.AddNode(nodes[i - 1]);

		REQUIRE(sg.GetGenerationsCount() == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(sg.GetGenerationSize(i) == 1);
			REQUIRE(sg.IsNodeExists(nodes[i]));
		}

		REQUIRE(!sg.IsValid(sg.GetParent(nodes[0])));
		REQUIRE(sg.GetFirstChild(nodes[0]).handle == nodes[1].handle);
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[0])));
		for (unsigned i = 1; i < SIZE - 1; ++i)
		{
			REQUIRE(sg.GetParent(nodes[i]).handle == nodes[i - 1].handle);
			REQUIRE(sg.GetFirstChild(nodes[i]).handle == nodes[i + 1].handle);
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
		REQUIRE(sg.GetParent(nodes[SIZE - 1]).handle == nodes[SIZE - 2].handle);
		REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[SIZE - 1])));
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[SIZE - 1])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[SIZE - 1])));
	}

	TEST_CASE("Add vertical 10")
	{
		constexpr unsigned SIZE = 10;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		nodes[0] = sg.AddNode();
		for (unsigned i = 1; i < SIZE; ++i)
			nodes[i] = sg.AddNode(nodes[i - 1]);

		REQUIRE(sg.GetGenerationsCount() == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(sg.GetGenerationSize(i) == 1);
			REQUIRE(sg.IsNodeExists(nodes[i]));
		}

		REQUIRE(!sg.IsValid(sg.GetParent(nodes[0])));
		REQUIRE(sg.GetFirstChild(nodes[0]).handle == nodes[1].handle);
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[0])));
		for (unsigned i = 1; i < SIZE - 1; ++i)
		{
			REQUIRE(sg.GetParent(nodes[i]).handle == nodes[i - 1].handle);
			REQUIRE(sg.GetFirstChild(nodes[i]).handle == nodes[i + 1].handle);
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
		REQUIRE(sg.GetParent(nodes[SIZE - 1]).handle == nodes[SIZE - 2].handle);
		REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[SIZE - 1])));
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[SIZE - 1])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[SIZE - 1])));
	}

	TEST_CASE("Add vertical 50")
	{
		constexpr unsigned SIZE = 50;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		nodes[0] = sg.AddNode();
		for (unsigned i = 1; i < SIZE; ++i)
			nodes[i] = sg.AddNode(nodes[i - 1]);

		REQUIRE(sg.GetGenerationsCount() == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(sg.GetGenerationSize(i) == 1);
			REQUIRE(sg.IsNodeExists(nodes[i]));
		}

		REQUIRE(!sg.IsValid(sg.GetParent(nodes[0])));
		REQUIRE(sg.GetFirstChild(nodes[0]).handle == nodes[1].handle);
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[0])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[0])));
		for (unsigned i = 1; i < SIZE - 1; ++i)
		{
			REQUIRE(sg.GetParent(nodes[i]).handle == nodes[i - 1].handle);
			REQUIRE(sg.GetFirstChild(nodes[i]).handle == nodes[i + 1].handle);
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
		REQUIRE(sg.GetParent(nodes[SIZE - 1]).handle == nodes[SIZE - 2].handle);
		REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[SIZE - 1])));
		REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[SIZE - 1])));
		REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[SIZE - 1])));
	}

	TEST_CASE("Remove horizontal root first")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}

		sg.RemoveNode(nodes[0]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE - 1);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		for (unsigned i = 1; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 1; i < SIZE; ++i)
		{
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
	}

	TEST_CASE("Remove horizontal root mid")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE - 1);
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		for (unsigned i = 0; i < 2; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
		for (unsigned i = 3; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 0; i < SIZE; ++i)
		{
			if (i == 2)
				continue;
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
	}

	TEST_CASE("Remove horizontal root last")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}

		sg.RemoveNode(nodes[SIZE - 1]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE - 1);
		REQUIRE(!sg.IsNodeExists(nodes[SIZE - 1]));
		for (unsigned i = 0; i < SIZE - 1; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 0; i < SIZE - 1; ++i)
		{
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}
	}

	TEST_CASE("Remove horizontal root all")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode();

		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 0; i < SIZE; ++i)
		{
			REQUIRE(!sg.IsValid(sg.GetParent(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetFirstChild(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetPrevSibling(nodes[i])));
			REQUIRE(!sg.IsValid(sg.GetNextSibling(nodes[i])));
		}

		for (unsigned i = 0; i < SIZE; ++i)
			sg.RemoveNode(nodes[i]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 0);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(!sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove horizontal child first")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle root = sg.AddNode();
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode(root);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == SIZE);
		REQUIRE(sg.IsNodeExists(root));
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == SIZE - 1);
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		for (unsigned i = 1; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove horizontal child mid")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle root = sg.AddNode();
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode(root);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == SIZE);
		REQUIRE(sg.IsNodeExists(root));
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == SIZE - 1);
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		for (unsigned i = 0; i < 2; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
		for (unsigned i = 3; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove horizontal child last")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle root = sg.AddNode();
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode(root);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == SIZE);
		REQUIRE(sg.IsNodeExists(root));
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[SIZE - 1]);
		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == SIZE - 1);
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(!sg.IsNodeExists(nodes[SIZE - 1]));
		for (unsigned i = 0; i < SIZE - 1; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove horizontal child all")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle root = sg.AddNode();
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);

		A3D::NodeHandle nodes[SIZE];
		for (unsigned i = 0; i < SIZE; ++i)
			nodes[i] = sg.AddNode(root);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == SIZE);
		REQUIRE(sg.IsNodeExists(root));
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		for (unsigned i = 0; i < SIZE; ++i)
			sg.RemoveNode(nodes[i]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.IsNodeExists(root));
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(!sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove vertical leaf")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		nodes[0] = sg.AddNode();
		for (unsigned i = 1; i < SIZE; ++i)
			nodes[i] = sg.AddNode(nodes[i - 1]);

		REQUIRE(sg.GetGenerationsCount() == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.GetGenerationSize(i) == 1);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[SIZE - 1]);
		REQUIRE(sg.GetGenerationsCount() == SIZE - 1);
		for (unsigned i = 0; i < SIZE - 1; ++i)
			REQUIRE(sg.GetGenerationSize(i) == 1);
		REQUIRE(!sg.IsNodeExists(nodes[SIZE - 1]));
		for (unsigned i = 0; i < SIZE - 1; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove vertical mid")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		nodes[0] = sg.AddNode();
		for (unsigned i = 1; i < SIZE; ++i)
			nodes[i] = sg.AddNode(nodes[i - 1]);

		REQUIRE(sg.GetGenerationsCount() == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.GetGenerationSize(i) == 1);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[SIZE - 2]);
		REQUIRE(sg.GetGenerationsCount() == SIZE - 2);
		for (unsigned i = 0; i < SIZE - 2; ++i)
			REQUIRE(sg.GetGenerationSize(i) == 1);
		for (unsigned i = 0; i < SIZE - 2; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
		for (unsigned i = SIZE - 2; i < SIZE; ++i)
			REQUIRE(!sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove vertical root")
	{
		constexpr unsigned SIZE = 5;

		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[SIZE];
		nodes[0] = sg.AddNode();
		for (unsigned i = 1; i < SIZE; ++i)
			nodes[i] = sg.AddNode(nodes[i - 1]);

		REQUIRE(sg.GetGenerationsCount() == SIZE);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.GetGenerationSize(i) == 1);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(1) == 0);
		for (unsigned i = 0; i < SIZE; ++i)
			REQUIRE(!sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove pyramid 2x2 root")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[3];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 2);

		for (unsigned i = 0; i < 3; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(1) == 0);
		for (unsigned i = 0; i < 3; ++i)
			REQUIRE(!sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove pyramid 2x4 mid")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[5];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);
		nodes[3] = sg.AddNode(nodes[0]);
		nodes[4] = sg.AddNode(nodes[0]);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 4);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 3);
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		for (unsigned i = 0; i < 2; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
		for (unsigned i = 3; i < 5; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove pyramid 2x4 root")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[5];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);
		nodes[3] = sg.AddNode(nodes[0]);
		nodes[4] = sg.AddNode(nodes[0]);

		REQUIRE(sg.GetGenerationsCount() == 2);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 4);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(1) == 0);
		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(!sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove pyramid 3x3 root")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[7];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);
		nodes[3] = sg.AddNode(nodes[1]);
		nodes[4] = sg.AddNode(nodes[1]);
		nodes[5] = sg.AddNode(nodes[2]);
		nodes[6] = sg.AddNode(nodes[2]);

		REQUIRE(sg.GetGenerationsCount() == 3);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 2);
		REQUIRE(sg.GetGenerationSize(2) == 4);

		for (unsigned i = 0; i < 7; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(1) == 0);
		for (unsigned i = 0; i < 7; ++i)
			REQUIRE(!sg.IsNodeExists(nodes[i]));
	}

	TEST_CASE("Remove pyramid 3x3 mid")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[7];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);
		nodes[3] = sg.AddNode(nodes[1]);
		nodes[4] = sg.AddNode(nodes[1]);
		nodes[5] = sg.AddNode(nodes[2]);
		nodes[6] = sg.AddNode(nodes[2]);

		REQUIRE(sg.GetGenerationsCount() == 3);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 2);
		REQUIRE(sg.GetGenerationSize(2) == 4);

		for (unsigned i = 0; i < 7; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.GetGenerationsCount() == 3);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 1);
		REQUIRE(sg.GetGenerationSize(2) == 2);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(sg.IsNodeExists(nodes[5]));
		REQUIRE(sg.IsNodeExists(nodes[6]));
	}

	TEST_CASE("Remove pyramid 3x3 mid all")
	{
		A3D::SceneGraph sg;

		A3D::NodeHandle nodes[7];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);
		nodes[3] = sg.AddNode(nodes[1]);
		nodes[4] = sg.AddNode(nodes[1]);
		nodes[5] = sg.AddNode(nodes[2]);
		nodes[6] = sg.AddNode(nodes[2]);

		REQUIRE(sg.GetGenerationsCount() == 3);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 2);
		REQUIRE(sg.GetGenerationSize(2) == 4);

		for (unsigned i = 0; i < 7; ++i)
			REQUIRE(sg.IsNodeExists(nodes[i]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.GetGenerationsCount() == 3);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.GetGenerationSize(1) == 1);
		REQUIRE(sg.GetGenerationSize(2) == 2);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(sg.IsNodeExists(nodes[5]));
		REQUIRE(sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.GetGenerationsCount() == 1);
		REQUIRE(sg.GetGenerationSize(0) == 1);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));
	}

	TEST_CASE("Set transform")
	{
		A3D::SceneGraph sg;
		const A3D::NodeHandle node = sg.AddNode();
		REQUIRE(sg.IsNodeExists(node));

		mat4 matrix1;
		for (unsigned i = 0; i < 4; ++i)
			for (unsigned j = 0; j < 4; ++j)
				matrix1[i][j] = (float)(i * 10 + j);
		sg.SetTransform(node, matrix1);

		const mat4& matrix2 = sg.GetGlobalTransform(node);
		for (unsigned i = 0; i < 4; ++i)
			for (unsigned j = 0; j < 4; ++j)
			{
				REQUIRE(matrix2[i][j] == matrix1[i][j]);
				REQUIRE(matrix2[i][j] == (float)(i * 10 + j));
			}
	}

	TEST_CASE("Update transform 2")
	{
		A3D::SceneGraph sg;
		const A3D::NodeHandle root = sg.AddNode();
		const A3D::NodeHandle leaf = sg.AddNode(root);
		REQUIRE(sg.IsNodeExists(root));
		REQUIRE(sg.IsNodeExists(leaf));

		mat4 root_matrix;
		mat4 leaf_matrix;
		mat4 result_matrix_expected;
		for (unsigned i = 0; i < 4; ++i)
			for (unsigned j = 0; j < 4; ++j)
			{
				root_matrix[j][j] = (float)(i * 10 + j);
				leaf_matrix[i][j] = (float)(i * 5 + j * 2);
			}
		glm_mat4_mul(root_matrix, leaf_matrix, result_matrix_expected);

		sg.SetTransform(root, root_matrix);
		sg.SetTransform(leaf, leaf_matrix);

		sg.UpdateTransformations();

		const mat4& result_matrix_real = sg.GetGlobalTransform(leaf);
		for (unsigned i = 0; i < 4; ++i)
			for (unsigned j = 0; j < 4; ++j)
				REQUIRE(result_matrix_real[i][j] == result_matrix_expected[i][j]);
	}
}
