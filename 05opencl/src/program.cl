/*
 * SortMatching
 * Copyright 2012 Marco Mandrioli
 *
 * This file is part of SortMatching.
 *
 * SortMatching is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SortMatching is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with SortMatching.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include "program.clh"


// calculates the bitwise NOT of the vector
__kernel void vector_bitwise_not(__global BITVECTOR_T *vec, const uint size)
{
	const uint gid = get_global_id(0);
	if (gid < size)
		vec[gid] = ~vec[gid];
}


// calculates the bitwise OR of the vector
__kernel void vector_bitwise_or(__global BITVECTOR_T *vec, __global BITVECTOR_T *mask, const uint size)
{
	const uint gid = get_global_id(0);
	if (gid < size)
		vec[gid] |= mask[gid];
}
