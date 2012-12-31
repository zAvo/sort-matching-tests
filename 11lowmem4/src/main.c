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


#include "../include/types.h"

#include "../include/matching.h"
#include "../include/test_generator.h"
#include "../include/utils.h"
#include "../include/error.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __TEST
#include <time.h>
#endif // __TEST


/** \mainpage
\author Marco Mandrioli

\section Intro1 Introdction

\section Intro2 Version
*/


/**file main.c
\brief File containing the main function.
*/


/** \brief Main function.
*/
int main(int argc, char *argv[])
{
	bitmatrix result;
	match_data_t data;
	_INT updates;
	_INT subscrs;
	_INT dimensions;
#ifdef __TEST
	FILE *fout;
	char fname[FILE_NAME_SIZE];
	clock_t start, end;
#endif // __TEST

	if ((argc == 2 && strcmp(argv[1], "--help") == 0) || argc != 4)
	{
		printf("\nSYNOPSIS:\n\n");
		printf("%s <updates> <subscriptions> <dimensions>\n\n", FILENAME(argv[0]));
		
		return (int)err_none;
	}

	updates = atoi(argv[1]);
	if (updates <= 0)
		printf("\nNot a valid number of update extents.\n");
	subscrs = atoi(argv[2]);
	if (subscrs <= 0)
		printf("\nNot a valid number of subscription extents.\n");
	dimensions = atoi(argv[3]);
	if (dimensions <= 0)
		printf("\nNot a valid number of dimensions.\n");
	
	if (updates <= 0 || subscrs <= 0 || dimensions <= 0)
	{
		set_error(err_invalid_input, __FILE__, __FUNCTION__, __LINE__);
		return (int)print_error_string();
	}

#ifdef __RANDOM_SET
	// generate a random data set
	if (test_generator_random(&data, updates, subscrs, dimensions) != err_none)
#else // __RANDOM_SET
	// generate a fixed data set
	if (test_generator(&data, updates, subscrs, dimensions) != err_none)
#endif // __RANDOM_SET
		return (int)print_error_string();

#ifdef __TEST
	// start test timer
	start = clock();
#endif // __TEST

	// allocate the result bit matrix
	if (create_bit_matrix(&result, data.size_update, data.size_subscr) != err_none)
		return (int)print_error_string();

	// main algorithm
	if (sort_matching(data, result) != err_none)
		return (int)print_error_string();

#ifdef __TEST
	// stop test timer
	end = clock();

#ifdef _WIN32
	// format output file name and open file
	if (sprintf_s(fname, FILE_NAME_SIZE, "%s_%d_%d_%d.txt", FILENAME(argv[0]), updates, subscrs, dimensions) == -1 || fopen_s(&fout, fname, "a+") != 0)
	{
		set_error(err_file, __FILE__, __FUNCTION__, __LINE__);
		return (int)print_error_string();
	}
#else // _WIN32
	// format output file name
	if (sprintf(fname, "%s_%d_%d_%d.txt", FILENAME(argv[0]), updates, subscrs, dimensions) < 0)
	{
		set_error(err_file, __FILE__, __FUNCTION__, __LINE__);
		return (int)print_error_string();
	}
	
	// open file
	fout = fopen(fname, "a+");
	if (fout == NULL)
	{
		set_error(err_file, __FILE__, __FUNCTION__, __LINE__);
		return (int)print_error_string();
	}
#endif // _WIN32

	// print output to file
	fprintf(fout, "%f\n", ((float)(end - start)) / CLOCKS_PER_SEC);

	fclose(fout);

#ifdef __DEBUG
	getchar();
#endif // __DEBUG
#endif // __TEST

#ifdef __VERBOSE
	// print the result bit matrix
	print_bitmatrix(result, data.size_update, data.size_subscr);

#ifdef __DEBUG
	getchar();
#endif // __DEBUG
#endif // __VERBOSE

#ifndef __NOFREE
	// free memory
	free(*result);
	free(result);
	free(data.update);
	free(data.subscr);
#endif // __NOFREE

	return (int)err_none;
}
