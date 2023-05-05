/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2023 Yuriy Zinchenko

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

#pragma once

#include <iterator>
#include <memory>
#include <utility>
#include "noexcept_allocator.h"

namespace A3D
{
template <typename Char = char, typename Allocator = A3D::noexcept_allocator<Char>>
class basic_string
{
public:
	using traits_type = void;
	using value_type = Char;
	using allocator_type = Allocator;
	using size_type = typename std::allocator_traits<Allocator>::size_type;
	using difference_type = typename std::allocator_traits<Allocator>::difference_type;
	using reference = Char&;
	using const_reference = const Char&;
	using pointer = typename std::allocator_traits<Allocator>::pointer;
	using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
	using iterator = pointer;
	using const_iterator = const_pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	basic_string() :
		str_(nullptr),
		refs_(nullptr),
		size_(0)
	{
	}

	explicit basic_string(Allocator&& alloc) :
		alloc_(std::forward<Allocator>(alloc)),
		str_(nullptr),
		refs_(nullptr),
		size_(0)
	{
	}

	basic_string(const_pointer str) { copy_new_str(str); }
	basic_string(const_pointer str, Allocator&& alloc) : alloc_(std::forward<Allocator>(alloc)) { copy_new_str(str); }

	basic_string(const basic_string& src) :
		alloc_(src.alloc_),
		refs_(src.refs_),
		str_(src.str_),
		size_(src.size_)
	{
		++*refs_;
	}

	basic_string(basic_string&& src) noexcept :
		alloc_(src.alloc_),
		refs_(src.refs_),
		str_(src.str_),
		size_(src.size_)
	{
		src.set_empty();
	}

	~basic_string() { delete_str(); }

	void operator=(const_pointer str)
	{
		delete_str();
		copy_new_str(str);
	}

	void operator=(const basic_string& src)
	{
		delete_str();
		alloc_ = src.alloc_;
		refs_ = src.refs_;
		str_ = src.str_;
		size_ = src.size_;
		++*refs_;
	}

	void operator=(basic_string&& src) noexcept
	{
		delete_str();
		alloc_ = src.alloc_;
		refs_ = src.refs_;
		str_ = src.str_;
		size_ = src.size_;
		src.set_empty();
	}

	basic_string operator+(const_pointer rhs) const
	{
		basic_string ret;
		ret.concat_string(str_, rhs, szstr_length(rhs));
		return ret;
	}

	basic_string operator+(const basic_string& rhs) const
	{
		basic_string ret;
		ret.concat_string(str_, rhs.str_, rhs.size_);
		return ret;
	}

	void operator+=(const_pointer rhs) { append_string(rhs, size_ + szstr_length(rhs)); }
	void operator+=(const basic_string& rhs) { append_string(rhs.c_str(), size_ + rhs.size_); }
	bool operator==(const_pointer rhs) const { return compare_string(rhs); }
	bool operator==(const basic_string& rhs) const { return compare_string(rhs.c_str()); }

	void clear()
	{
		delete_str();
		set_empty();
	}

	bool empty() const { return size_ == 0; }
	const_pointer c_str() const { return str_; }
	const_pointer data() const { return str_; }
	size_type size() const { return size_; }
	size_type length() const { return size(); }
	size_type refs() const { return *refs_; }

	pointer front() { return str_; }
	pointer back() { return str_ + size_; }
	const_pointer front() const { return str_; }
	const_pointer back() const { return str_ + size_; }
	reference at(size_type index) { return str_[index]; }
	const_reference at(size_type index) const { return str_[index]; }
	reference operator[](size_type index) { return at(index); }
	const_reference operator[](size_type index) const { return at(index); }

	iterator begin() { return front(); }
	iterator end() { return back() + 1; }
	const_iterator begin() const { return front(); }
	const_iterator end() const { return back() + 1; }
	const_iterator cbegin() { return front(); }
	const_iterator cend() { return back() + 1; }
	const_iterator cbegin() const { return front(); }
	const_iterator cend() const { return back() + 1; }
	reverse_iterator rbegin() { return std::make_reverse_iterator(end()); }
	reverse_iterator rend() { return std::make_reverse_iterator(begin()); }
	const_reverse_iterator rbegin() const { return std::make_reverse_iterator(end()); }
	const_reverse_iterator rend() const { return std::make_reverse_iterator(begin()); }
	const_reverse_iterator crbegin() { return std::make_reverse_iterator(end()); }
	const_reverse_iterator crend() { return std::make_reverse_iterator(begin()); }
	const_reverse_iterator crbegin() const { return std::make_reverse_iterator(end()); }
	const_reverse_iterator crend() const { return std::make_reverse_iterator(begin()); }

private:
	void alloc_memory(pointer& str, size_type*& refs, size_type length)
	{
		pointer data = alloc_.allocate(size_ + 1 + sizeof(*refs_));
		str = data + sizeof(*refs);
		refs = (size_type*)data;
		*refs = 1;
	}

	void alloc_new_str(size_type length)
	{
		alloc_memory(str_, refs_, length);
	}

	void delete_str()
	{
		if (refs_)
		{
			--*refs_;
			if (*refs_ == 0)
				alloc_.deallocate((pointer)refs_, size_ + 1 + sizeof(*refs_));
		}
	}

	void copy_new_str(const_pointer str)
	{
		size_ = szstr_length(str);
		alloc_new_str(size_);
		const_pointer src = str;
		pointer dst = str_;
		for (; src < str + size_; ++src, ++dst)
			*dst = *src;
		*dst = '\0';
	}

	void set_empty()
	{
		refs_ = nullptr;
		str_ = nullptr;
		size_ = 0;
	}

	void append_string(const_pointer rhs, size_type rhs_size)
	{
		size_ = size_ + rhs_size;
		pointer newstr;
		size_type* newrefs;
		alloc_memory(newstr, newrefs, size_);
		pointer dstchr = newstr;
		const_pointer srcchr = str_;
		while (*srcchr != '\0')
		{
			*dstchr = *srcchr;
			++srcchr;
			++dstchr;
		}
		srcchr = rhs;
		while (*srcchr != '\0')
		{
			*dstchr = *srcchr;
			++srcchr;
			++dstchr;
		}
		*dstchr = '\0';
		delete_str();
		str_ = newstr;
		refs_ = newrefs;
	}

	void concat_string(const_pointer lhs, const_pointer rhs, size_type result_size)
	{
		size_ = result_size;
		alloc_new_str(size_);
		pointer dstchr = str_;
		const_pointer srcchr = lhs;
		while (*srcchr != '\0')
		{
			*dstchr = *srcchr;
			++srcchr;
			++dstchr;
		}
		srcchr = rhs;
		while (*srcchr != '\0')
		{
			*dstchr = *srcchr;
			++srcchr;
			++dstchr;
		}
		*dstchr = '\0';
	}

	bool compare_string(const_pointer rhs) const
	{
		const_pointer lhs = str_;
		while (*lhs == '\0' && *rhs == '\0')
		{
			++lhs;
			++rhs;
		}
		return *lhs == *rhs;
	}

	static size_type szstr_length(const_pointer str)
	{
		size_type count = 0;
		while (*str != '\0')
		{
			++count;
			++str;
		}
		return count;
	}

	pointer str_;
	size_type* refs_;
	size_type size_;
	allocator_type alloc_;
};

using string = basic_string<char, A3D::noexcept_allocator<char>>;
} // namespace A3D

namespace std
{
template <typename T, typename Allocator>
struct hash<A3D::basic_string<T, Allocator>>
{
	size_t operator()(const A3D::basic_string<T, Allocator>& str) const noexcept
	{
		auto chr = str.begin();
		size_t hash = 0;
		while (*chr)
		{
			hash += (size_t)*chr;
			hash += (hash << 10Lu);
			hash ^= (hash << 6Lu);
			++chr;
		}
		hash += (hash << 3Lu);
		hash ^= (hash << 11Lu);
		hash += (hash << 15Lu);
		return hash;
	}
};
}
