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

#ifndef ARRAYHASHMAP_H
#define ARRAYHASHMAP_H

#include <cstdio>
#include <iterator>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

template <typename TKey, typename TValue> struct ArrayHashMapPair
{
	TKey first;
	TValue second;
};

template <typename TKey> struct EmptyKeyFunctor
{
};

template <typename TKey,
		  typename TValue,
		  typename THash = std::hash<TKey>,
		  typename TKeyEqual = std::equal_to<TKey>,
		  typename TKeyEmpty = EmptyKeyFunctor<TKey>,
		  typename TAlloc = std::allocator<ArrayHashMapPair<TKey, TValue>>>
class ArrayHashMap
{
public:
	using KeyType = TKey;
	using MappedType = TValue;
	using ValueType = ArrayHashMapPair<TKey, TValue>;
	using SizeType = unsigned;
	using DifferenceType = int;
	using Hasher = THash;
	using KeyEqual = TKeyEqual;
	using KeyEmpty = TKeyEmpty;
	using AllocType = TAlloc;
	using Reference = ValueType&;
	using ConstReference = const ValueType&;
	using Pointer = ValueType*;
	using ConstPointer = const ValueType*;

	class ConstIterator;

	class Iterator
		: public std::iterator<std::random_access_iterator_tag, ValueType, DifferenceType, Pointer, Reference>
	{
	public:
		Iterator() noexcept
			: ptr_(nullptr)
		{
		}
		Iterator(ValueType* ptr) noexcept
			: ptr_(ptr)
		{
		}
		Iterator(const Iterator& src) noexcept
			: ptr_(src.ptr_)
		{
		}

		void operator=(const Iterator& rhs) noexcept { ptr_ = rhs.ptr_; }
		void operator=(ValueType* ptr) noexcept { ptr_ = ptr; }

		ValueType& operator*() const noexcept { return *ptr_; }
		ValueType* operator->() const noexcept { return ptr_; }
		void operator++() noexcept { ++ptr_; }
		void operator--() noexcept { --ptr_; }
		bool operator==(const ValueType* rhs) const noexcept { return ptr_ == rhs; }
		bool operator!=(const ValueType* rhs) const noexcept { return ptr_ != rhs; }
		bool operator<(const ValueType* rhs) const noexcept { return ptr_ < rhs; }
		bool operator>(const ValueType* rhs) const noexcept { return ptr_ > rhs; }
		bool operator<=(const ValueType* rhs) const noexcept { return ptr_ <= rhs; }
		bool operator>=(const ValueType* rhs) const noexcept { return ptr_ >= rhs; }
		bool operator==(const Iterator& rhs) const noexcept { return ptr_ == rhs.ptr_; }
		bool operator!=(const Iterator& rhs) const noexcept { return ptr_ != rhs.ptr_; }
		bool operator<(const Iterator& rhs) const noexcept { return ptr_ < rhs.ptr_; }
		bool operator>(const Iterator& rhs) const noexcept { return ptr_ > rhs.ptr_; }
		bool operator<=(const Iterator& rhs) const noexcept { return ptr_ <= rhs.ptr_; }
		bool operator>=(const Iterator& rhs) const noexcept { return ptr_ >= rhs.ptr_; }
		bool operator==(const ConstIterator& rhs) const noexcept { return ptr_ == rhs.ptr_; }
		bool operator!=(const ConstIterator& rhs) const noexcept { return ptr_ != rhs.ptr_; }
		bool operator<(const ConstIterator& rhs) const noexcept { return ptr_ < rhs.ptr_; }
		bool operator>(const ConstIterator& rhs) const noexcept { return ptr_ > rhs.ptr_; }
		bool operator<=(const ConstIterator& rhs) const noexcept { return ptr_ <= rhs.ptr_; }
		bool operator>=(const ConstIterator& rhs) const noexcept { return ptr_ >= rhs.ptr_; }
		Iterator operator+(DifferenceType rhs) const noexcept { return ptr_ + rhs; }
		DifferenceType operator-(const Iterator& rhs) const noexcept { return ptr_ - rhs.ptr_; }
		DifferenceType operator-(const ConstIterator& rhs) const noexcept { return ptr_ - rhs.ptr_; }
		Pointer Ptr() const noexcept { return ptr_; }

		friend class ConstIterator;

	private:
		Pointer ptr_;
	};

	struct ConstIterator
		: public std::iterator<std::random_access_iterator_tag, ValueType, DifferenceType, ConstPointer, ConstReference>
	{
	public:
		ConstIterator() noexcept
			: ptr_(nullptr)
		{
		}
		ConstIterator(const ValueType* ptr) noexcept
			: ptr_(ptr)
		{
		}
		ConstIterator(const Iterator& src) noexcept
			: ptr_(src.ptr_)
		{
		}
		ConstIterator(const ConstIterator& src) noexcept
			: ptr_(src.ptr_)
		{
		}

		void operator=(const Iterator& rhs) noexcept { ptr_ = rhs.ptr_; }
		void operator=(const ConstIterator& rhs) noexcept { ptr_ = rhs.ptr_; }
		void operator=(ValueType* ptr) noexcept { ptr_ = ptr; }

		const ValueType& operator*() const noexcept { return *ptr_; }
		const ValueType* operator->() const noexcept { return ptr_; }
		void operator++() noexcept { ++ptr_; }
		void operator--() noexcept { --ptr_; }
		bool operator==(const ValueType* rhs) const noexcept { return ptr_ == rhs; }
		bool operator!=(const ValueType* rhs) const noexcept { return ptr_ != rhs; }
		bool operator<(const ValueType* rhs) const noexcept { return ptr_ < rhs; }
		bool operator>(const ValueType* rhs) const noexcept { return ptr_ > rhs; }
		bool operator<=(const ValueType* rhs) const noexcept { return ptr_ <= rhs; }
		bool operator>=(const ValueType* rhs) const noexcept { return ptr_ >= rhs; }
		bool operator==(const Iterator& rhs) const noexcept { return ptr_ == rhs.ptr_; }
		bool operator!=(const Iterator& rhs) const noexcept { return ptr_ != rhs.ptr_; }
		bool operator<(const Iterator& rhs) const noexcept { return ptr_ < rhs.ptr_; }
		bool operator>(const Iterator& rhs) const noexcept { return ptr_ > rhs.ptr_; }
		bool operator<=(const Iterator& rhs) const noexcept { return ptr_ <= rhs.ptr_; }
		bool operator>=(const Iterator& rhs) const noexcept { return ptr_ >= rhs.ptr_; }
		bool operator==(const ConstIterator& rhs) const noexcept { return ptr_ == rhs.ptr_; }
		bool operator!=(const ConstIterator& rhs) const noexcept { return ptr_ != rhs.ptr_; }
		bool operator<(const ConstIterator& rhs) const noexcept { return ptr_ < rhs.ptr_; }
		bool operator>(const ConstIterator& rhs) const noexcept { return ptr_ > rhs.ptr_; }
		bool operator<=(const ConstIterator& rhs) const noexcept { return ptr_ <= rhs.ptr_; }
		bool operator>=(const ConstIterator& rhs) const noexcept { return ptr_ >= rhs.ptr_; }
		ConstIterator operator+(DifferenceType rhs) const noexcept { return ptr_ + rhs; }
		DifferenceType operator-(const Iterator& rhs) const noexcept { return ptr_ - rhs.ptr_; }
		DifferenceType operator-(const ConstIterator& rhs) const noexcept { return ptr_ - rhs.ptr_; }
		ConstPointer Ptr() const noexcept { return ptr_; }

	private:
		ConstPointer ptr_;
	};

	ArrayHashMap()
		: data_(nullptr)
		, size_(0)
	{
	}

	explicit ArrayHashMap(const THash& hash,
						  const TKeyEqual& equal = TKeyEqual(),
						  const TKeyEmpty& empty = TKeyEmpty(),
						  const TAlloc& alloc = TAlloc())
		: data_(nullptr)
		, hasher_(hash)
		, keyEqual_(equal)
		, keyEmpty_(empty)
		, alloc_(alloc)
		, size_(0)
	{
	}

	ArrayHashMap(SizeType bucketCount, const TAlloc& alloc)
		: ArrayHashMap(bucketCount, Hasher(), TKeyEqual(), TKeyEmpty(), alloc)
	{
	}

	ArrayHashMap(SizeType bucketCount, const THash& hash, const TAlloc& alloc = TAlloc())
		: ArrayHashMap(bucketCount, hash, KeyEqual(), TKeyEmpty(), alloc)
	{
	}

	explicit ArrayHashMap(const TAlloc& alloc)
		: data_(nullptr)
		, alloc_(alloc)
		, size_(0)
	{
	}

	template <class InputIterator>
	ArrayHashMap(InputIterator first,
				 InputIterator last,
				 const THash& hash = THash(),
				 const TKeyEqual& equal = TKeyEqual(),
				 const TKeyEmpty& empty = TKeyEmpty(),
				 const TAlloc& alloc = TAlloc())
		: hasher_(hash)
		, keyEqual_(equal)
		, keyEmpty_(empty)
		, alloc_(alloc)
		, size_(std::distance(first, last))
	{
		data_ = Alloc(size_);
		Construct(data_, size_);
		Pointer ptr;
		while (first != last)
		{
			ptr = GetPositionEmpty(data_, size_, first->first);
			ptr->first = first->first;
			new (&ptr->second) MappedType(first->second);
			++first;
		}
	}

	template <class InputIterator>
	ArrayHashMap(InputIterator first, InputIterator last, const TAlloc& alloc)
		: ArrayHashMap(first, last, Hasher(), KeyEqual(), KeyEmpty(), alloc)
	{
	}

	template <class InputIterator>
	ArrayHashMap(InputIterator first, InputIterator last, const THash& hash, const TAlloc& alloc)
		: ArrayHashMap(first, last, hash, KeyEqual(), KeyEmpty(), alloc)
	{
	}

	ArrayHashMap(const ArrayHashMap& other)
		: ArrayHashMap(other, other.alloc_)
	{
	}

	ArrayHashMap(const ArrayHashMap& other, const TAlloc& alloc)
		: hasher_(other.hasher_)
		, keyEqual_(other.keyEqual_)
		, keyEmpty_(other.keyEmpty_)
		, alloc_(alloc)
		, size_(other.size_)
	{
		data_ = Alloc(size_);
		Construct(data_, size_);
		Copy(other.data_, data_, size_);
	}

	ArrayHashMap(ArrayHashMap&& other) noexcept
		: ArrayHashMap(std::move(other), other.alloc_)
	{
	}

	ArrayHashMap(ArrayHashMap&& other, const TAlloc& alloc) noexcept
		: data_(other.data_)
		, hasher_(std::move(other.hasher_))
		, keyEqual_(std::move(other.keyEqual_))
		, keyEmpty_(std::move(other.keyEmpty_))
		, alloc_(alloc)
		, size_(other.size_)
	{
		other.data_ = nullptr;
		other.size_ = 0;
	}

	ArrayHashMap(std::initializer_list<ValueType>&& init,
				 const THash& hash = THash(),
				 const TKeyEqual& equal = TKeyEqual(),
				 const TKeyEmpty& empty = TKeyEmpty(),
				 const TAlloc& alloc = TAlloc())
		: ArrayHashMap(init.begin(), init.end(), hash, equal, empty, alloc)
	{
	}

	ArrayHashMap(std::initializer_list<ValueType>&& init, const TAlloc& alloc)
		: ArrayHashMap(std::move(init), Hasher(), TKeyEqual(), TKeyEmpty(), alloc)
	{
	}

	ArrayHashMap(std::initializer_list<ValueType>&& init, const THash& hash, const TAlloc& alloc)
		: ArrayHashMap(std::move(init), hash, KeyEqual(), TKeyEmpty(), alloc)
	{
	}

	~ArrayHashMap()
	{
		Destruct(data_, size_);
		Free(data_, size_);
	}

	ArrayHashMap& operator=(const ArrayHashMap& other)
	{
		Clear();
		Free(data_, size_);
		size_ = other.size_;
		data_ = Alloc(size_);
		Construct(data_, size_);
		Copy(other.data_, data_, size_);
		return *this;
	}

	ArrayHashMap& operator=(ArrayHashMap&& other) noexcept
	{
		data_ = other.data_;
		size_ = other.size_;
		other.data_ = nullptr;
		other.size_ = 0;
		hasher_ = std::move(hasher_);
		keyEqual_ = std::move(keyEqual_);
		return *this;
	}

	ArrayHashMap& operator=(std::initializer_list<ValueType> other)
	{
		Clear();
		Free(data_, size_);
		size_ = other.size();
		data_ = Alloc(size_);
		Construct(data_, size_);
		Pointer dstptr;
		for (auto srcIt = other.begin(); srcIt != other.end(); ++srcIt)
		{
			dstptr = GetPositionEmpty(data_, size_, srcIt->first);
			dstptr->first = std::move(srcIt->first);
			new (&dstptr->second) MappedType(srcIt->second);
		}
		return *this;
	}

	Iterator Begin() noexcept { return First(); }
	ConstIterator Begin() const noexcept { return First(); }
	ConstIterator CBegin() noexcept { return First(); }

	Iterator End() noexcept { return Last(); }
	ConstIterator End() const noexcept { return Last(); }
	ConstIterator CEnd() noexcept { return Last(); }

	[[nodiscard]] bool Empty() const noexcept { return size_ == 0; }

	SizeType Size() const noexcept { return size_; }

	void Clear() noexcept
	{
		Destruct(data_, size_);
		Free(data_, size_);
		data_ = nullptr;
		size_ = 0;
	}

	std::pair<Iterator, bool> Insert(const ValueType& value) { return Emplace(value.first, value.second); }

	std::pair<Iterator, bool> Insert(ValueType&& value)
	{
		return Emplace(std::move(value.first), std::move(value.second));
	}

	template <typename InputIterator> SizeType Insert(InputIterator first, InputIterator last)
	{
		Resize(size_ + std::distance(first, last));
		SizeType inserted = 0;
		Pointer ptr;
		while (first != last)
		{
			ptr = GetPositionEmpty(data_, size_, first->first);
			ptr->first = first->first;
			new (&ptr->second) MappedType(first->second);
			++inserted;
			++first;
		}
		return inserted;
	}

	SizeType Insert(std::initializer_list<ValueType> ilist) { return Insert(ilist.begin(), ilist.end()); }

	template <typename Key, typename... Args> std::pair<Iterator, bool> Emplace(Key&& key, Args&&... args)
	{
		Resize(size_ + 1);
		Pointer ptr = GetPositionEmpty(data_, size_, key);
		ptr->first = std::forward<Key>(key);
		new (&ptr->second) MappedType(std::forward<Args>(args)...);
		return std::make_pair(Iterator(ptr), true);
	}

	Iterator Erase(Iterator pos)
	{
		DifferenceType id = EraseSwap(data_, size_, pos.Ptr());
		Resize(size_ - 1);
		return data_ + id;
	}

	ConstIterator Erase(ConstIterator pos)
	{
		DifferenceType id = EraseSwap(data_, size_, pos.Ptr());
		Resize(size_ - 1);
		return data_ + id;
	}

	SizeType Erase(const KeyType& key)
	{
		Iterator it = Find(key);
		if (it != End())
		{
			Erase(it);
			return 1;
		}
		else
			return 0;
	}

	template <typename IteratorsIterator> SizeType EraseKeys(IteratorsIterator first, IteratorsIterator last)
	{
		SizeType count = 0;
		Pointer end;
		Iterator curr;
		while (first != last)
		{
			curr = Find(first->first);
			if (curr != End())
			{
				end = LastUsed();
				curr->first = std::move(end->first);
				curr->second = std::move(end->second);
				++count;
				--size_;
			}
			++first;
		}
		if (count)
		{
			size_ += count;
			Resize(size_ - count);
		}
		return count;
	}

	void Swap(ArrayHashMap& other) noexcept
	{
		ArrayHashMap temp;
		temp.data_ = data_;
		temp.hasher_ = hasher_;
		temp.keyEqual_ = keyEqual_;
		temp.alloc_ = alloc_;
		temp.size_ = size_;
		data_ = other.data_;
		hasher_ = other.hasher_;
		keyEqual_ = other.keyEqual_;
		alloc_ = other.alloc_;
		size_ = other.size_;
		other.data_ = temp.data_;
		other.hasher_ = temp.hasher_;
		other.keyEqual_ = temp.keyEqual_;
		other.alloc_ = temp.alloc_;
		other.size_ = temp.size_;
	}

	ValueType& At(const KeyType& key) { return Find(key)->second; }

	const ValueType& At(const KeyType& key) const { return Find(key)->second; }

	MappedType& operator[](const KeyType& key)
	{
		Iterator it = Find(key);
		if (it != End())
			return it->second;
		else
			return Last()->second;
	}

	const MappedType& operator[](const KeyType& key) const { return Find(key)->second; }

	Iterator Find(const KeyType& key)
	{
		Pointer ptr = GetPositionBusy(data_, size_, key);
		return ptr ? ptr : End();
	}

	ConstIterator Find(const KeyType& key) const
	{
		Pointer ptr = GetPositionBusy(data_, size_, key);
		return ptr ? ptr : End();
	}

	bool Contains(const KeyType& key) const { return Find(key) != End(); }

	Hasher HashFunction() const { return hasher_; }
	KeyEqual KeyEq() const { return keyEqual_; }

	Iterator begin() noexcept { return Begin(); }
	ConstIterator begin() const noexcept { return Begin(); }
	Iterator end() noexcept { return End(); }
	ConstIterator end() const noexcept { return End(); }

	void DebugPrint() const
	{
		printf("Size:     %u\n", size_);
		for (unsigned i = 0; i < size_; ++i)
			printf("(%d, %d) ", data_[i].first, data_[i].second.value_);
		printf("\n");
	}

private:
	Pointer First() const noexcept { return data_; }
	Pointer Last() const noexcept { return data_ + size_; }
	Pointer LastUsed() const noexcept { return Last() - 1; }

	Pointer GetPositionEmpty(ValueType* data, SizeType size, const KeyType& key) const noexcept
	{
		const size_t hash = size ? hasher_(key) % size : 0;
		const Pointer start = data + hash;
		Pointer ptr = start;
		do
		{
			if (keyEmpty_(ptr->first) || keyEqual_(ptr->first, key))
				return ptr;
			++ptr;
			if (ptr == data + size)
				ptr = data;
		} while (ptr != start);
		return nullptr;
	}

	Pointer GetPositionBusy(ValueType* data, SizeType size, const KeyType& key) const noexcept
	{
		if (size_)
		{
			const SizeType hash = static_cast<SizeType>(hasher_(key)) % size;
			const Pointer start = data + hash;
			Pointer ptr = start;
			do
			{
				if (keyEqual_(ptr->first, key))
					return ptr;
				++ptr;
				if (ptr == data + size)
					ptr = data;
			} while (ptr != start);
		}
		return nullptr;
	}

	[[nodiscard]] ValueType* Alloc(SizeType count) noexcept { return alloc_.allocate(count); }

	void Free(ValueType* data, SizeType count) noexcept { alloc_.deallocate(data, count); }

	void Construct(ValueType* data, SizeType count) const noexcept
	{
		for (Pointer ptr = data; ptr < data + count; ++ptr)
		{
			new (&ptr->first) KeyType;
			keyEmpty_.Initialize(ptr->first);
		}
	}

	void Destruct(ValueType* data, SizeType count) const noexcept
	{
		for (Pointer ptr = data; ptr < data + count; ++ptr)
		{
			ptr->first.~KeyType();
			ptr->second.~MappedType();
		}
	}

	void Move(ValueType* src, SizeType srcSize, ValueType* dst, SizeType dstSize) noexcept
	{
		const SizeType size = srcSize < dstSize ? srcSize : dstSize;
		Pointer dstptr;
		for (Pointer srcptr = src; srcptr < src + size; ++srcptr)
		{
			dstptr = GetPositionEmpty(dst, dstSize, srcptr->first);
			dstptr->first = std::move(srcptr->first);
			new (&dstptr->second) MappedType(std::move(srcptr->second));
		}
	}

	void Move(ValueType* src, ValueType* dst, SizeType size) noexcept
	{
		Pointer srcptr = src;
		Pointer dstptr = dst;
		while (srcptr < src + size)
		{
			dstptr->first = std::move(srcptr->first);
			new (&dstptr->second) MappedType(std::move(srcptr->second));
			++srcptr;
			++dstptr;
		}
	}

	void Copy(ConstPointer src, SizeType srcSize, Pointer dst, SizeType dstSize) noexcept
	{
		Pointer dstptr;
		for (ConstPointer srcptr = src; srcptr < src + srcSize; ++srcptr)
		{
			dstptr = GetPositionEmpty(dstptr, dstSize, srcptr->first);
			dstptr->first = srcptr->first;
			new (&dstptr->second) MappedType(srcptr->second);
		}
	}

	void Copy(ConstPointer src, Pointer dst, SizeType size) noexcept
	{
		ConstPointer srcptr = src;
		Pointer dstptr = dst;
		while (srcptr < src + size)
		{
			dstptr->first = srcptr->first;
			new (&dstptr->second) MappedType(srcptr->second);
			++srcptr;
			++dstptr;
		}
	}

	DifferenceType EraseSwap(ValueType* data, SizeType size, Pointer ptr) const noexcept
	{
		DifferenceType pos = ptr - data_;
		Pointer last = data_ + size_ - 1;
		ptr->first = std::move(last->first);
		ptr->second = std::move(last->second);
		return pos;
	}

	void Resize(SizeType count)
	{
		ValueType* newData = Alloc(count);
		Construct(newData, count);
		if (size_)
		{
			Move(data_, size_, newData, count);
			Destruct(data_, size_);
			Free(data_, size_);
		}
		data_ = newData;
		size_ = count;
	}

	ValueType* data_;
	AllocType alloc_;
	Hasher hasher_;
	KeyEqual keyEqual_;
	KeyEmpty keyEmpty_;
	SizeType size_;
};

#endif // ARRAYHASHMAP_H

template <> struct EmptyKeyFunctor<int>
{
	bool operator()(int key) const noexcept { return key == -1; }
	void Initialize(int& key) const noexcept { key = -1; }
};

template <> struct EmptyKeyFunctor<unsigned>
{
	bool operator()(unsigned key) const noexcept { return key == ~0u; }
	void Initialize(unsigned& key) const noexcept { key = ~0u; }
};

template <> struct EmptyKeyFunctor<long long>
{
	bool operator()(long long key) const noexcept { return key == -1L; }
	void Initialize(long long& key) const noexcept { key = -1L; }
};

template <> struct EmptyKeyFunctor<unsigned long long>
{
	bool operator()(unsigned long long key) const noexcept { return key == ~0LU; }
	void Initialize(unsigned long long& key) const noexcept { key = ~0LU; }
};

template <> struct EmptyKeyFunctor<std::string>
{
	bool operator()(const std::string& key) const noexcept { return key.empty(); }
	void Initialize(std::string&) const noexcept {}
};
