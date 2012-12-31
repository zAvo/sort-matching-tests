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


/* This set of headers is put before everything else because pthread.h requires it, otherwise its behaviour can be undefined.
*/
#ifdef _MSC_VER
#include <process.h>
#include <Windows.h>

#define THREAD_T HANDLE
#define MUTEX_T HANDLE
#define RETVAL_T DWORD
#else // _MSC_VER
#define _MULTI_THREADED
#include <pthread.h>

#define THREAD_T pthread_t
#define MUTEX_T pthread_mutex_t
#define RETVAL_T _ERR_CODE *
#endif // _MSC_VER

#include "../include/types.h"

#include "../include/utils.h"
#include "../include/error.h"

#include <stdlib.h>
#include <string.h>


/** \file matching.c
\brief File containing the main matching algorithm functions.
*/


/** \brief Thread arguments structure.

This structure contains all the parameters needed by the threads.
*/
typedef struct {
	match_data_t	data;			///< data of the problem
	MUTEX_T			*line_mutex;	///< array of mutexes (one for each line of the bit matrix)
	bitmatrix		out;			///< output bit matrix
	_UINT			dimension;		///< dimension to be calculated
} thread_params;


#ifdef _MSC_VER
/** \brief One-dimensional matching.

This function performs the sort matching on a single dimension. It's the start routine of the threads created by the function sort_matching().

\remarks This is the Windows version of the function. The error codes are casted to unsigned int.

\param pVoid a void pointer to the structure containing the parameters

\retval error code
*/
unsigned int __stdcall sort_matching_1D(void *pVoid)
#else // _MSC_VER
/** \brief One-dimensional matching.

This function performs the sort matching on a single dimension. It's the start routine of the threads created by the function sort_matching().

\remarks This is the Linux version of the function.

\param pVoid a void pointer to the structure containing the parameters

\retval a pointer to the error code, or NULL if an error happened during the allocation of the return code memory
*/
void *sort_matching_1D(void *pVoid)
#endif // _MSC_VER
{
	_UINT i;
	_UINT bit_pos;
	_UINT line;
	_UINT line_width;
	_UINT list_size;
	_UINT update_ep_count;
	thread_params params;
	list_ptr ep_list;
	bitvector subscr_set_before;
	bitvector subscr_set_after;
	
#ifndef _MSC_VER
	_ERR_CODE *err;
	
	// allocate the memory for the return value
	err = (_ERR_CODE *)malloc(sizeof(_ERR_CODE));
	if (err == NULL)
		return NULL;
#endif // _MSC_VER

	params = *(thread_params *)pVoid;

	line_width = BIT_VEC_WIDTH(params.data.size_subscr);

	// two endpoints for each extent
	list_size = (params.data.size_update + params.data.size_subscr) * 2;

	// allocate the "list"
	ep_list = (list_ptr)malloc(list_size * sizeof(list_t));
	
	// allocate the two subscription extents sets (setting no subscription extent to "before" with calloc)
	subscr_set_before = (bitvector)calloc(line_width, sizeof(bitvec_elem));
	subscr_set_after = (bitvector)malloc(line_width * sizeof(bitvec_elem));
	
	if (ep_list == NULL || subscr_set_before == NULL || subscr_set_after == NULL)
	{
#ifdef _MSC_VER
		_endthreadex((unsigned int)err_alloc);
		return (unsigned int)err_alloc;
#else // _MSC_VER
		*err = err_alloc;
		pthread_exit((void *)err);
		return (void *)err;
#endif // _MSC_VER
	}

	// fill the endpoints "list" with the data of the dimension to be processed
	set_endpoints_list(params.data, ep_list, params.dimension);

	// number of elements on each line of the bit matrix
	line_width = BIT_VEC_WIDTH(params.data.size_subscr);
	// number of endpoints of update extents
	update_ep_count = params.data.size_update * 2;

	// sort the endpoints list
	sort_list(ep_list, list_size);

	// set all the subscription extents to "after"
	memset(subscr_set_after, 0xFF, line_width * sizeof(bitvec_elem));
	
	// for each endpoint in the list, but stops when all update extents endpoints are processed
	// (only when processing update extents endpoints the algorithm writes on the matching matrix)
	for (i = 0; update_ep_count > 0; i++)
	{
		// if it's the endpoint of a subscription extent
		if (ep_list[i].id < params.data.size_subscr)
		{
			// calculate the element in the bit vector that contains the bit
			bit_pos = BIT_TO_POS(ep_list[i].id);
			
			// if it's the lower endpoint
			if (ep_list[i].is_lower_point)
			{
				// clear the bit in the bit vector (remove the subscription extent from the "after" set)
				BIT_CLEAR(subscr_set_after[bit_pos], DBIT(BIT_POS_IN_VEC(ep_list[i].id, bit_pos)));
			}
			else // if it's the upper endpoint
			{
				// set the bit in the bit vector (add the subscription extent to the "before" set)
				BIT_SET(subscr_set_before[bit_pos], DBIT(BIT_POS_IN_VEC(ep_list[i].id, bit_pos)));
			}
		}
		else // if it's the endpoint of an update extent
		{
			update_ep_count--;

			line = ep_list[i].id - params.data.size_subscr;
			
#ifdef _MSC_VER
			// wait for mutex if the line is in use from another thread
			if (WaitForSingleObject(params.line_mutex[line], INFINITE) != WAIT_OBJECT_0)
			{
				_endthreadex((unsigned int)err_threads);
				return (unsigned int)err_threads;
			}
#else // _MSC_VER
			// wait for mutex if the line is in use from another thread
			if (pthread_mutex_lock(&params.line_mutex[line]) != 0)
			{
				*err = err_threads;
				pthread_exit((void *)err);
				return (void *)err;
			}
#endif // _MSC_VER

			// if it's the lower endpoint
			if (ep_list[i].is_lower_point)
			{
				// bitwise OR (write all the subscription extents in the "before" set in the update extent's line in the bit matrix)
				vector_bitwise_or(params.out[line], subscr_set_before, line_width);
			}
			else // if it's the upper endpoint
			{
				// bitwise OR (write all the subscription extents in the "after" set in the update extent's line in the bit matrix)
				vector_bitwise_or(params.out[line], subscr_set_after, line_width);
			}

#ifdef _MSC_VER
			// release mutex
			if (ReleaseMutex(params.line_mutex[line]) == 0)
			{
				_endthreadex((unsigned int)err_threads);
				return (unsigned int)err_threads;
			}
#else // _MSC_VER
			// release mutex
			if (pthread_mutex_unlock(&params.line_mutex[line]) != 0)
			{
				*err = err_threads;
				pthread_exit((void *)err);
				return (void *)err;
			}
#endif // _MSC_VER
		}
	}

#ifndef __NOFREE
	// free memory
	free(ep_list);
	free(subscr_set_before);
	free(subscr_set_after);
#endif // __NOFREE

#ifdef _MSC_VER
	_endthreadex((unsigned int)err_none);
	return (unsigned int)err_none;
#else // _MSC_VER
	*err = err_none;
	pthread_exit((void *)err);
	return (void *)err;
#endif // _MSC_VER
}


/** \brief Main algorithm function.

This function performs all the operations needed to feed the data one dimension at a time to the matching_1D function.

\param data the data set
\param out the output bit matrix

\retval error code
*/
_ERR_CODE sort_matching(const match_data_t data, const bitmatrix out)
{
	_UINT i;
	_UINT line_width;
	_UINT matrix_size;
	THREAD_T thread[MAX_DIMENSIONS];
	MUTEX_T *line_mutex;
	thread_params params[MAX_DIMENSIONS];
	RETVAL_T retval = 0;
#ifndef _MSC_VER
	_ERR_CODE retcpy;
#endif // _MSC_VER

	line_width = BIT_VEC_WIDTH(data.size_subscr);
	matrix_size = data.size_update * line_width;

	if (data.dimensions < 1)
		return set_error(err_invalid_input, __FILE__, __FUNCTION__, __LINE__);

	if (data.dimensions > MAX_DIMENSIONS)
		return set_error(err_too_many_dim, __FILE__, __FUNCTION__, __LINE__);

	// allocate memory for the mutexes array (one mutex for every line of the bit matrix)
	line_mutex = (MUTEX_T *)malloc(data.size_update * sizeof(MUTEX_T));
	if (line_mutex == NULL)
		return set_error(err_alloc, __FILE__, __FUNCTION__, __LINE__);

	for (i = 0; i < data.size_update; i++)
	{
#ifdef _MSC_VER
		// create (initialize) mutexes
		line_mutex[i] = CreateMutex(NULL, FALSE, NULL);
		if (line_mutex[i] == NULL)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);
#else // _MSC_VER
		// create (initialize) mutexes
		if (pthread_mutex_init(&line_mutex[i], NULL) != 0)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);
#endif // _MSC_VER
	}

	// for each dimension
	for (i = 0; i < data.dimensions; i++)
	{
		// set the parameters for the i-th thread
		params[i].data = data;
		params[i].line_mutex = line_mutex;
		params[i].out = out;
		params[i].dimension = i;

#ifdef _MSC_VER
		// create and start the thread
		thread[i] = (HANDLE)_beginthreadex(NULL, 0U, sort_matching_1D, &params[i], _P_NOWAIT, NULL);
		if (thread[i] == NULL)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);
#else // _MSC_VER
		// create and start the thread
		if (pthread_create(&thread[i], NULL, sort_matching_1D, &params[i]) != 0)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);
#endif // _MSC_VER
	}

#ifdef _MSC_VER
	// wait for all threads to finish
	if (WaitForMultipleObjects(data.dimensions, thread, TRUE, INFINITE) == WAIT_FAILED)
		return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);

#ifndef __NOFREE
	// for each line in the bit matrix
	for (i = 0; i < data.size_update; i++)
	{
		// close mutex handle
		if (CloseHandle(line_mutex[i]) == 0)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);
	}
	// free the mutexes array
	free(line_mutex);
#endif // __NOFREE

	// for each thread
	for (i = 0; i < data.dimensions; i++)
	{
		// get the exit code of the thread
		if (GetExitCodeThread(thread[i], &retval) == 0)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);
		// check for errors in exit code
		if ((_ERR_CODE)retval != err_none)
			return (_ERR_CODE)retval;

#ifndef __NOFREE
		// close thread handle
		if (CloseHandle(thread[i]) == 0)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);
#endif // __NOFREE
	}
#else // _MSC_VER
	// for each thread
	for (i = 0; i < data.dimensions; i++)
	{
		// wait for all threads to finish (and get the exit code of the thread)
		if (pthread_join(thread[i], (void *)&retval) != 0)
			return set_error(err_threads, __FILE__, __FUNCTION__, __LINE__);

		// if a thread returns NULL it means something has gone wrong during the allocation of the memory for the return code
		if (retval == NULL)
		{
			return set_error(err_alloc, __FILE__, __FUNCTION__, __LINE__);
		}
		else
		{
			retcpy = *retval;
#ifndef __NOFREE
		// free the memory allocated by the thread
			free(retval);
#endif // __NOFREE
		// check for errors in exit code
			if (retcpy != err_none)
				return retcpy;
		}
	}

#ifndef __NOFREE
	// for each line in the bit matrix, destroy the mutex
	for (i = 0; i < data.size_update; i++)
		pthread_mutex_destroy(&line_mutex[i]);

	// free the mutexes array
	free(line_mutex);
#endif // __NOFREE
#endif // _MSC_VER

	// bitwise NOT of the non-matching table to obtain the matching table
	vector_bitwise_not(out[0], matrix_size);

	return err_none;
}
