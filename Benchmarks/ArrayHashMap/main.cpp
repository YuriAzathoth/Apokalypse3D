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
#include <array>
#include <string>
#include <unordered_map>
#include "Container/ArrayHashMap.h"

static constexpr size_t ITERATION_MAP_SIZE = 1024;

CELERO_MAIN

struct MapValue
{
	MapValue() = default;
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

std::array<MapValue, ITERATION_MAP_SIZE> g_data;
ArrayHashMap<size_t, MapValue> g_hm;
std::unordered_map<size_t, MapValue> g_um;

struct AI0
{
	AI0()
	{
		for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
			g_data[i] = MapValue((float)i, (float)i);
	}
} AI0;
struct AI1
{
	AI1()
	{
		for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
			g_hm.Emplace(i, MapValue((float)i, (float)i));
	}
} AI1;
struct AI2
{
	AI2()
	{
		for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
			g_um.emplace(i, MapValue((float)i, (float)i));
	}
} AI2;

BASELINE(Iteration, ArrayHashMap, 0, 0)
{
	for (auto it = g_hm.Begin(); it != g_hm.End(); ++it)
	{
		it->second.b += it->second.a;
		it->second.x = it->second.a + it->second.b;
	}
}

BASELINE(Finding, ArrayHashMap, 0, 0)
{
	float a = 0.0f, b = 0.0f, x = 0.0f;
	decltype(g_hm)::ConstIterator it;
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
	{
		it = g_hm.Find(i);
		a += it->second.a;
		b += it->second.b;
		x += it->second.x;
	}
}

BASELINE(Edition, ArrayHashMap, 0, 0)
{
	ArrayHashMap<size_t, MapValue> hm;
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.Emplace(i, MapValue((float)i, (float)i));
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.Erase(i);
}

BENCHMARK(Iteration, UnorderedMap, 0, 0)
{
	for (auto it = g_um.begin(); it != g_um.end(); ++it)
	{
		it->second.b += it->second.a;
		it->second.x = it->second.a + it->second.b;
	}
}

BENCHMARK(Finding, UnorderedMap, 0, 0)
{
	float a = 0.0f, b = 0.0f, x = 0.0f;
	decltype(g_um)::const_iterator it;
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
	{
		it = g_um.find(i);
		a += it->second.a;
		b += it->second.b;
		x += it->second.x;
	}
}

BENCHMARK(Edition, UnorderedMap, 0, 0)
{
	std::unordered_map<size_t, MapValue> hm;
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.emplace(i, MapValue((float)i, (float)i));
	for (size_t i = 0; i < ITERATION_MAP_SIZE; ++i)
		hm.erase(i);
}
