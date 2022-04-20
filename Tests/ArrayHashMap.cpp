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

#include <doctest/doctest.h>
#include <iostream>
#include "Container/ArrayHashMap.h"

#define TEST_DATA_LIST                                                                                                 \
	{                                                                                                                  \
		{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, { 6, 6 }                                                               \
	}

#define TEST_DATA_LIST_AUX                                                                                             \
	{                                                                                                                  \
		{5, 5}, {7, 7}, {8, 8}, {10, 10},                                                                              \
	}

static constexpr std::pair<int, int> TEST_DATA[] = TEST_DATA_LIST;
static constexpr std::pair<int, int> TEST_DATA_AUX[] = TEST_DATA_LIST_AUX;

static int g_AllocCount = 0;

template <typename T> struct DebugObject
{
	DebugObject() noexcept { ++g_AllocCount; }
	DebugObject(int value) noexcept
		: value_(value)
	{
		++g_AllocCount;
	}
	DebugObject(const DebugObject& src) noexcept
		: value_(src.value_)
	{
		++g_AllocCount;
	}
	DebugObject(DebugObject&& src) noexcept
		: value_(std::move(src.value_))
	{
		++g_AllocCount;
	}
	~DebugObject() { --g_AllocCount; }
	DebugObject& operator=(const DebugObject& src) noexcept
	{
		value_ = src.value_;
		return *this;
	}
	DebugObject& operator=(DebugObject&& src) noexcept
	{
		value_ = std::move(src.value_);
		return *this;
	}
	bool operator==(const DebugObject& rhs) const noexcept { return value_ == rhs.value_; }
	T value_;
};

struct MemoryLeakChecker
{
	~MemoryLeakChecker() { REQUIRE(g_AllocCount == 0); }
};

TEST_SUITE("ArrayHashMap")
{
	TEST_CASE("Default constructor")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		CHECK(hm.Empty());
		CHECK(hm.Size() == 0);
		CHECK(hm.Begin() == hm.End());
		CHECK(hm.CBegin() == hm.CEnd());
	}

	TEST_CASE("Emplace")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		for (const auto& p : TEST_DATA)
			hm.Emplace(p.first, p.second);
		REQUIRE(hm.Size() == std::size(TEST_DATA));
		REQUIRE(g_AllocCount == hm.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			CHECK(hm[p.first].value_ == p.second);
		}
	}

	TEST_CASE("Contains")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		hm.Emplace(1, 2);
		CHECK(!hm.Contains(0));
		REQUIRE(hm.Contains(1));
		CHECK(hm[1].value_ == 2);
	}

	TEST_CASE("Clear")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		for (const auto& p : TEST_DATA)
			hm.Emplace(p.first, p.second);
		REQUIRE(g_AllocCount == std::size(TEST_DATA));
		hm.Clear();
		REQUIRE(hm.Size() == 0);
		REQUIRE(g_AllocCount == 0);
	}

	TEST_CASE("Copy constructor")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> src;
		for (const auto& p : TEST_DATA)
			src.Emplace(p.first, p.second);

		ArrayHashMap<int, DebugObject<int>, -1> dst(src);
		REQUIRE(src.Size() == dst.Size());
		REQUIRE(g_AllocCount == src.Size() + dst.Size());
		auto it1 = src.Begin();
		auto it2 = dst.Begin();
		while (it1 != src.End())
		{
			CHECK(it1->first == it2->first);
			CHECK(it1->second == it2->second);
			++it1;
			++it2;
		}
	}

	TEST_CASE("Move constructor")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> src;
		for (const auto& p : TEST_DATA)
			src.Emplace(p.first, p.second);

		const unsigned srcsize = src.Size();
		ArrayHashMap<int, DebugObject<int>, -1> dst(std::move(src));
		REQUIRE(src.Size() == 0);
		REQUIRE(dst.Size() == srcsize);
		REQUIRE(g_AllocCount == dst.Size());
		auto it1 = src.Begin();
		auto it2 = dst.Begin();
		while (it1 != src.End())
		{
			CHECK(it1->first == it2->first);
			CHECK(it1->second == it2->second);
			++it1;
			++it2;
		}
	}

	TEST_CASE("Init list constructor")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm = TEST_DATA_LIST;

		REQUIRE(hm.Size() == std::size(TEST_DATA));
		REQUIRE(g_AllocCount == hm.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			CHECK(hm[p.first] == p.second);
		}
	}

	TEST_CASE("Range iterators constructor")
	{
		MemoryLeakChecker checker;
		std::pair<int, int> src[std::size(TEST_DATA)];
		for (unsigned i = 0; i < std::size(TEST_DATA); ++i)
			src[i] = TEST_DATA[i];

		ArrayHashMap<int, DebugObject<int>, -1> dst(std::begin(src), std::end(src));
		REQUIRE(dst.Size() == std::size(TEST_DATA));
		REQUIRE(g_AllocCount == dst.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(dst.Contains(p.first));
			CHECK(dst[p.first] == p.second);
		}
	}

	TEST_CASE("Copy assignment")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> src;
		for (const auto& p : TEST_DATA)
			src.Emplace(p.first, p.second);

		ArrayHashMap<int, DebugObject<int>, -1> dst;
		dst = src;
		REQUIRE(src.Size() == dst.Size());
		REQUIRE(g_AllocCount == src.Size() + dst.Size());
		auto it1 = src.Begin();
		auto it2 = dst.Begin();
		while (it1 != src.End())
		{
			CHECK(it1->first == it2->first);
			CHECK(it1->second == it2->second);
			++it1;
			++it2;
		}
	}

	TEST_CASE("Move assignment")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> src;
		for (const auto& p : TEST_DATA)
			src.Emplace(p.first, p.second);

		const unsigned srcsize = src.Size();
		ArrayHashMap<int, DebugObject<int>, -1> dst(std::move(src));
		REQUIRE(src.Size() == 0);
		REQUIRE(dst.Size() == srcsize);
		REQUIRE(g_AllocCount == dst.Size());
		auto it1 = src.Begin();
		auto it2 = dst.Begin();
		while (it1 != src.End())
		{
			CHECK(it1->first == it2->first);
			CHECK(it1->second == it2->second);
			++it1;
			++it2;
		}
	}

	TEST_CASE("Init list assignment")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		hm = TEST_DATA_LIST;

		REQUIRE(hm.Size() == std::size(TEST_DATA));
		REQUIRE(g_AllocCount == hm.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			CHECK(hm[p.first] == p.second);
		}
	}

	TEST_CASE("Copy insert")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		{
			ArrayHashMapPair<int, DebugObject<int>> pair;
			for (const auto& p : TEST_DATA)
			{
				pair.first = p.first;
				pair.second = p.second;
				hm.Insert(pair);
			}
		}

		REQUIRE(hm.Size() == std::size(TEST_DATA));
		REQUIRE(g_AllocCount == hm.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			CHECK(hm[p.first] == p.second);
		}
	}

	TEST_CASE("Move insert")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		for (const auto& p : TEST_DATA)
		{
			ArrayHashMapPair<int, DebugObject<int>> pair{p.first, p.second};
			hm.Insert(std::move(pair));
		}

		REQUIRE(hm.Size() == std::size(TEST_DATA));
		REQUIRE(g_AllocCount == hm.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			CHECK(hm[p.first] == p.second);
		}
	}

	TEST_CASE("Range iterators insert")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> src(std::begin(TEST_DATA), std::end(TEST_DATA));
		ArrayHashMap<int, DebugObject<int>, -1> dst(std::begin(TEST_DATA_AUX), std::end(TEST_DATA_AUX));
		REQUIRE(dst.Insert(src.Begin(), src.End()) == src.Size());

		REQUIRE(dst.Size() == src.Size() + std::size(TEST_DATA_AUX));
		REQUIRE(g_AllocCount == src.Size() + dst.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(dst.Contains(p.first));
			CHECK(dst[p.first] == p.second);
		}
		for (const auto& p : TEST_DATA_AUX)
		{
			REQUIRE(dst.Contains(p.first));
			CHECK(dst[p.first] == p.second);
		}
	}

	TEST_CASE("Init list insert")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm(std::begin(TEST_DATA), std::end(TEST_DATA));
		REQUIRE(hm.Insert(TEST_DATA_LIST_AUX) == std::size(TEST_DATA_AUX));

		REQUIRE(hm.Size() == std::size(TEST_DATA) + std::size(TEST_DATA_AUX));
		REQUIRE(g_AllocCount == hm.Size());
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			CHECK(hm[p.first] == p.second);
		}
		for (const auto& p : TEST_DATA_AUX)
		{
			REQUIRE(hm.Contains(p.first));
			CHECK(hm[p.first] == p.second);
		}
	}

	TEST_CASE("Key erase")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		for (const auto& p : TEST_DATA)
			hm.Emplace(p.first, p.second);
		auto it = hm.Begin();
		unsigned allocs = hm.Size();
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			hm.Erase(p.first);
			REQUIRE(!hm.Contains(p.first));
			REQUIRE(g_AllocCount == --allocs);
		}
		REQUIRE(g_AllocCount == 0);
	}

	TEST_CASE("Iterator erase")
	{
		MemoryLeakChecker checker;
		ArrayHashMap<int, DebugObject<int>, -1> hm;
		for (const auto& p : TEST_DATA)
			hm.Emplace(p.first, p.second);
		auto it = hm.Begin();
		unsigned allocs = hm.Size();
		for (const auto& p : TEST_DATA)
		{
			REQUIRE(hm.Contains(p.first));
			it = hm.Find(p.first);
			REQUIRE(it != hm.End());
			hm.Erase(it);
			REQUIRE(!hm.Contains(p.first));
			REQUIRE(g_AllocCount == --allocs);
		}
		REQUIRE(g_AllocCount == 0);
	}
}
