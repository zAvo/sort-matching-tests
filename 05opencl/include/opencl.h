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


#ifndef __OPENCL_H
#define __OPENCL_H


/** \file opencl.h
\brief Header of file opencl.c

The file opencl.c contains the functions that handle OpenCL.
*/


_ERR_CODE cl_init(const _UINT size_update, const _UINT size_subscr);
void vector_bitwise_not(const bitvector vector, const _UINT size);
void vector_bitwise_or(const bitvector vector, const bitvector mask, const _UINT size);

#ifndef __NOFREE
void cl_free();
#endif // __NOFREE


#endif // __OPENCL_H
