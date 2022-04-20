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

#define CELERO_STATIC
#include <celero/Celero.h>
#include <string>
#include <unordered_map>
#include "Container/ArrayHashMap.h"

static constexpr size_t ITERATION_MAP_SIZE = 1024;

CELERO_MAIN

struct MapValue
{
	MapValue(float _a, float _b)
		: a(_a)
		, b(_b)
		, x(0.0f)
	{
	}
	float a;
	float b;
	float x;
};

ArrayHashMap<size_t, MapValue> g_hm;
std::unordered_map<size_t, MapValue> g_um;

struct AI1
{
	AI1()
	{
		for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
			g_hm.Emplace(i, MapValue((float)i, (float)i));
	}
} ai1;
struct AI2
{
	AI2()
	{
		for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
			g_um.emplace(i, MapValue((float)i, (float)i));
	}
} ai2;

BASELINE(ArrayHashMap, Iteration, 10, 1000)
{
	for (auto it = g_hm.Begin(); it != g_hm.End(); ++it)
	{
		it->second.b += it->second.a;
		it->second.x = it->second.a + it->second.b;
	}
}

BENCHMARK(ArrayHashMap, Edition, 10, 10)
{
	ArrayHashMap<size_t, MapValue> hm;
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.Emplace(i, MapValue((float)i, (float)i));
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.Erase(i);
}

BENCHMARK(ArrayHashMap, RangedEdition, 10, 10)
{
	ArrayHashMap<size_t, MapValue> hm;
	hm.Insert(g_hm.Begin(), g_hm.End());
	hm.EraseKeys(g_hm.Begin(), g_hm.End());
}

BASELINE(UnorderedMap, Iteration, 10, 1000)
{
	for (auto it = g_um.begin(); it != g_um.end(); ++it)
	{
		it->second.b += it->second.a;
		it->second.x = it->second.a + it->second.b;
	}
}

BENCHMARK(UnorderedMap, Edition, 10, 10)
{
	std::unordered_map<size_t, MapValue> hm;
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.emplace(i, MapValue((float)i, (float)i));
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.erase(i);
}
