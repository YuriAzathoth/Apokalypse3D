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

#ifndef LAZYINIT_H
#define LAZYINIT_H

#include <new>
#include <utility>

template <typename T> class LazyInit
{
public:
	LazyInit()
		: init_(false)
	{
	}
	~LazyInit() { Destroy(); }

	template <typename... TArgs> void Initialize(TArgs&&... args)
	{
		Destroy();
		new (Get()) T(std::forward<TArgs>(args)...);
		init_ = true;
	}
	void Destroy()
	{
		if (init_)
		{
			Get()->~T();
			init_ = false;
		}
	}

	T* Get() noexcept { return reinterpret_cast<T*>(memory_); }
	T* operator*() noexcept { return Get(); }
	T* operator->() noexcept { return Get(); }
	const T* Get() const noexcept { return reinterpret_cast<const T*>(memory_); }
	const T* operator*() const noexcept { return Get(); }
	const T* operator->() const noexcept { return Get(); }

	bool NotEmpty() const noexcept { return init_; }
	bool Empty() const noexcept { return !NotEmpty(); }
	operator bool() const noexcept { return NotEmpty(); }

	LazyInit(const LazyInit& src) { Initialize(*src.Get()); }
	LazyInit(LazyInit&& src) noexcept
	{
		Initialize(std::move(*src.Get()));
		src.Destroy();
		src.init_ = false;
	}
	void operator=(const LazyInit& src) { Initialize(*src.Get()); }
	void operator=(LazyInit&& src) noexcept { Initialize(std::move(*src.Get())); }

private:
	char memory_[sizeof(T)];
	bool init_;
};

#endif // LAZYINIT_H
