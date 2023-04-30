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
	}

	TEST_CASE("Add")
	{
		A3D::SceneGraph sg;
		const A3D::NodeHandle node = sg.AddNode();
		REQUIRE(sg.IsNodeExists(node));
	}

	TEST_CASE("Remove")
	{
		A3D::SceneGraph sg;
		const A3D::NodeHandle node = sg.AddNode();
		REQUIRE(sg.IsNodeExists(node));

		sg.RemoveNode(node);
		REQUIRE(!sg.IsNodeExists(node));
	}

	TEST_CASE("Horizontal 2")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[2];

		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode();
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
	}

	TEST_CASE("Horizontal 5")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[5];

		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode();
		nodes[2] = sg.AddNode();
		nodes[3] = sg.AddNode();
		nodes[4] = sg.AddNode();
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[4]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[3]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
	}

	TEST_CASE("Vertical 2 leaf")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[2];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
	}

	TEST_CASE("Vertical 5 leaf")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[5];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[1]);
		nodes[3] = sg.AddNode(nodes[2]);
		nodes[4] = sg.AddNode(nodes[3]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[4]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[3]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
	}

	TEST_CASE("Vertical 3 mid")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[3];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
	}

	TEST_CASE("Vertical 5 mid")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[5];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[1]);
		nodes[3] = sg.AddNode(nodes[2]);
		nodes[4] = sg.AddNode(nodes[3]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
	}

	TEST_CASE("Vertical 2 root")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[2];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
	}

	TEST_CASE("Vertical 3 root")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[3];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
	}

	TEST_CASE("Vertical 5 root")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[5];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[1]);
		nodes[3] = sg.AddNode(nodes[2]);
		nodes[4] = sg.AddNode(nodes[3]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
	}

	TEST_CASE("Pyramid 2x2 remove leaf")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[3];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
	}

	TEST_CASE("Pyramid 2x3 remove leaf")
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
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));
		REQUIRE(sg.IsNodeExists(nodes[5]));
		REQUIRE(sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[6]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));
		REQUIRE(sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[5]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[3]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[4]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[2]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));
	}

	TEST_CASE("Pyramid 2x3 remove mid")
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
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));
		REQUIRE(sg.IsNodeExists(nodes[5]));
		REQUIRE(sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[1]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(sg.IsNodeExists(nodes[5]));
		REQUIRE(sg.IsNodeExists(nodes[6]));
	}

	TEST_CASE("Pyramid 2x2 remove root")
	{
		A3D::SceneGraph sg;
		A3D::NodeHandle nodes[3];
		nodes[0] = sg.AddNode();
		nodes[1] = sg.AddNode(nodes[0]);
		nodes[2] = sg.AddNode(nodes[0]);
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
	}

	TEST_CASE("Pyramid 2x3 remove root")
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
		REQUIRE(sg.IsNodeExists(nodes[0]));
		REQUIRE(sg.IsNodeExists(nodes[1]));
		REQUIRE(sg.IsNodeExists(nodes[2]));
		REQUIRE(sg.IsNodeExists(nodes[3]));
		REQUIRE(sg.IsNodeExists(nodes[4]));
		REQUIRE(sg.IsNodeExists(nodes[5]));
		REQUIRE(sg.IsNodeExists(nodes[6]));

		sg.RemoveNode(nodes[0]);
		REQUIRE(!sg.IsNodeExists(nodes[0]));
		REQUIRE(!sg.IsNodeExists(nodes[1]));
		REQUIRE(!sg.IsNodeExists(nodes[2]));
		REQUIRE(!sg.IsNodeExists(nodes[3]));
		REQUIRE(!sg.IsNodeExists(nodes[4]));
		REQUIRE(!sg.IsNodeExists(nodes[5]));
		REQUIRE(!sg.IsNodeExists(nodes[6]));
	}

	/*TEST_CASE("Set transform")
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

		const mat4& result_matrix_fact = sg.GetGlobalTransform(leaf);
		for (unsigned i = 0; i < 4; ++i)
			for (unsigned j = 0; j < 4; ++j)
				REQUIRE(result_matrix_fact[i][j] == result_matrix_expected[i][j]);
	}*/
}
