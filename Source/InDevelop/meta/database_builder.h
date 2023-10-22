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

#ifndef CONTAINER_META_DATABASE_BUILDER_H
#define CONTAINER_META_DATABASE_BUILDER_H

#include "database.h"

namespace A3D
{
namespace db
{
template <typename SizeType = uint16_t,
		  typename IndexTypesList = meta::types_list_builder<>,
		  typename IndexTagsList = meta::types_list_builder<>,
		  typename DataTypesList = meta::types_list_builder<>,
		  typename TagsTypesList = meta::types_list_builder<>,
		  typename Allocator = std::allocator<uint8_t>>
struct database_builder
{
	using primary_index_type = SizeType;
	using secondary_index_types_list = IndexTypesList;
	using secondary_index_tags_list = IndexTagsList;
	using data_types_list = DataTypesList;
	using data_tags_list = TagsTypesList;
	using allocator_type = Allocator;

	template <typename T>
	using primary_key =
	database_builder
	<
		T,
		secondary_index_types_list,
		secondary_index_tags_list,
		data_types_list,
		data_tags_list,
		allocator_type
	>;

	template <typename T, typename U>
	using secondary_key =
	database_builder
	<
		primary_index_type,
		typename secondary_index_types_list::add<T>,
		typename secondary_index_tags_list::add<U>,
		data_types_list,
		data_tags_list,
		allocator_type
	>;

	template <typename Tag, typename T>
	using data =
	database_builder
	<
		primary_index_type,
		secondary_index_types_list,
		secondary_index_tags_list,
		typename data_types_list::add<T>,
		typename data_tags_list::add<Tag>,
		allocator_type
	>;

	using build = database
	<
		primary_index_type,
		uint32_t,
		typename data_types_list::add<primary_index_type>::type
		typename meta::list_join<typename data_types_list::add<primary_index_type>::type, secondary_index_types_list>::type,
		typename meta::list_join<typename data_tags_list::add<primary_key>::type, secondary_index_tags_list>::type,
		allocator_type
	>;
};
} // namespace db
} // namespace A3D

#endif // CONTAINER_META_DATABASE_BUILDER_H
