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

#include "../include/error.h"

#include <CL/cl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/** \file opencl.c
\brief File containing the functions that handle OpenCL.
*/


cl_command_queue queue;
cl_kernel kernel_not;
cl_kernel kernel_or;
cl_mem vec;
cl_mem vec2;
size_t local_ws = LOCAL_WS;
size_t global_ws;


/** \brief Performs a bitwise NOT on a vector using OpenCL.

This function tells OpenCL to write data to the input buffer, to enqueue the kernel and then to read data back from the output buffer.

\param vector the bit vector to be inverted
\param size the size (in number of elements, not bits) of the bit vector
*/
void vector_bitwise_not(const bitvector vector, const _UINT size)
{
	// write the input buffer
	clEnqueueWriteBuffer(queue, vec, CL_TRUE, 0, size * sizeof(bitvec_elem), vector, 0, NULL, NULL);

	// calculate global size
	global_ws = GLOBAL_WS(size);

	// enqueue kernel
	clEnqueueNDRangeKernel(queue, kernel_not, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL);

	// read the output buffer
	clEnqueueReadBuffer(queue, vec, CL_TRUE, 0, size * sizeof(bitvec_elem), vector, 0, NULL, NULL);

}


/** \brief Performs a bitwise OR on two vectors using OpenCL.

This function tells OpenCL to write data to the input buffers, to enqueue the kernel and then to read data back from the output buffer.

\param vector the bit vector to perform the OR on, output data can be read on it
\param mask the bit vector used as a mask for the OR operation
\param size the size (in number of elements, not bits) of the bit vectors
*/
void vector_bitwise_or(const bitvector vector, const bitvector mask, const _UINT size)
{
	// write the input buffers
	clEnqueueWriteBuffer(queue, vec, CL_TRUE, 0, size * sizeof(bitvec_elem), vector, 0, NULL, NULL);
	clEnqueueWriteBuffer(queue, vec2, CL_TRUE, 0, size * sizeof(bitvec_elem), mask, 0, NULL, NULL);
	
	// calculate global size
	global_ws = GLOBAL_WS(size);

	// enqueue kernel
	clEnqueueNDRangeKernel(queue, kernel_or, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL);

	// read the output buffer
	clEnqueueReadBuffer(queue, vec, CL_TRUE, 0, size * sizeof(bitvec_elem), vector, 0, NULL, NULL);
}


/** \brief OpenCL initialization function.

This function sets up the OpenCL platform and device, reads the OpenCL program from file, builds the kernels in it, creates the buffers and sets them as kernels arguments.

\param size_update the number of update extents
\param size_subscr the number of subscription extents

\retval error code
*/
_ERR_CODE cl_init(const _UINT size_update, const _UINT size_subscr)
{
	cl_uint num_platforms;
	cl_platform_id *platforms;
	
	cl_uint num_devices;
	cl_device_id *devices;
	
	char vendor_name[VENDOR_NAME_SIZE];

	cl_device_id device;
	cl_context context;
	cl_program program;
	cl_int err;

	FILE *program_handle;
	_BYTE *program_buffer;
	size_t program_size;
	size_t size;
	_UINT i, j;
	_BOOL device_found;

#ifdef __VERBOSE
	size_t log_size;
	_BYTE program_log[BUILD_LOG_SIZE_MAX];
#endif // __VERBOSE
	
	// search for a device (GPU) with expected vendor name on a platform (SDK) with the same vendor name
	// set the device_found variable to 'not found'
	device_found = FALSE;

	// get the number of OpenCL platforms (SDKs)
	if (clGetPlatformIDs(0, NULL, &num_platforms) != CL_SUCCESS || num_platforms == 0)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// allocate num_platforms OpenCL platforms (SDKs)
	platforms = (cl_platform_id *)malloc(num_platforms * sizeof(cl_platform_id));
	if (platforms == NULL)
		return set_error(err_alloc, __FILE__, __FUNCTION__, __LINE__);

	// get the IDs of the OpenCL platforms (SDKs)
	if (clGetPlatformIDs(num_platforms, platforms, NULL) != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// for all platforms (SDKs)
	for (i = 0; i < num_platforms && !device_found; i++)
	{
		// get the vendor name string of this platform (SDK)
		if (clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, VENDOR_NAME_SIZE, vendor_name, NULL) != CL_SUCCESS)
			return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

		// check whether the vendor name is the expected one (nVidia? AMD? Other?)
		if (strcmp(vendor_name, VIDEO_CARD_VENDOR) == 0)
		{
			// get the number of OpenCL devices of type GPU on this platform
			if (clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices) != CL_SUCCESS || num_devices == 0)
				return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

			// allocate num_devices OpenCL devices (GPUs)
			devices = (cl_device_id *)malloc(num_devices * sizeof(cl_device_id));
			if (devices == NULL)
				return set_error(err_alloc, __FILE__, __FUNCTION__, __LINE__);

			// get the IDs of the OpenCL devices (GPUs) on this platform
			if (clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, num_devices, devices, NULL) != CL_SUCCESS)
				return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

			// for all devices (GPUs) on this platform
			for (j = 0; j < num_devices && !device_found; j++)
			{
				// get the vendor name string of this device (GPU)
				if (clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, VENDOR_NAME_SIZE, vendor_name, NULL) != CL_SUCCESS)
				return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

				// check whether the vendor name is the expected one (nVidia? AMD? Other?)
				if (strcmp(vendor_name, VIDEO_CARD_VENDOR) == 0)
				{
					// device found
					device_found = TRUE;
					device = devices[j];
				}
			}

			// free this platform device IDs memory
			free(devices);
		}
	}

	// free the platform IDs memory
	free(platforms);

	if (!device_found)
		return set_error(err_opencl_device_not_found, __FILE__, __FUNCTION__, __LINE__);

	// create context for that device
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// open OpenCL program file (open as byte file because ftell wouldn't work correctly otherwise)
#ifdef _MSC_VER
#pragma message (__FILE__ "(" TOSTR(__LINE__) "): warning: if run from Visual Studio, you need to set the Configuration Properties->Debugging->Working Directory to $(OutDir)")
#pragma message (__FILE__ "(" TOSTR(__LINE__) "): warning: you need to copy program.cl and program.clh in the same directory as the .exe")

	if (fopen_s(&program_handle, PROGRAM_FILE, "rb") != 0)
		return set_error(err_opencl_file, __FILE__, __FUNCTION__, __LINE__);
#else // _MSC_VER
	program_handle = fopen(PROGRAM_FILE, "rb");
	if (program_handle == NULL)
		return set_error(err_opencl_file, __FILE__, __FUNCTION__, __LINE__);
#endif // _MSC_VER

	// seek to the end of the file
	if (fseek(program_handle, 0, SEEK_END) != 0)
		return set_error(err_opencl_file, __FILE__, __FUNCTION__, __LINE__);
	// get program size
	program_size = ftell(program_handle);
	if (program_size == -1)
		return set_error(err_opencl_file, __FILE__, __FUNCTION__, __LINE__);
	// rewind to the beginning of the file
	rewind(program_handle);
	
	// allocate buffer for the program code
	program_buffer = (_BYTE *)malloc(program_size + 1);
	if (program_buffer == NULL)
		return set_error(err_alloc, __FILE__, __FUNCTION__, __LINE__);
	program_buffer[program_size] = '\0';

	// transfer data from the file to the buffer
	if (fread(program_buffer, sizeof(_BYTE), program_size, program_handle) != program_size)
		return set_error(err_opencl_file, __FILE__, __FUNCTION__, __LINE__);
	
	// close file handler
	fclose(program_handle); // ignoring EOF error on closing
	
	// create a program from the source
	program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer, &program_size, &err);
	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// free the buffer
	free(program_buffer);

	// build the program
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

#ifdef __VERBOSE
	// get build log
	log_size = BUILD_LOG_SIZE_MAX;
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, program_log, NULL);
	printf("Build log: %s\n", program_log);
#endif // __VERBOSE

	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// create kernels
	kernel_not = clCreateKernel(program, KERNEL_BITWISE_NOT, &err);
	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);
	kernel_or = clCreateKernel(program, KERNEL_BITWISE_OR, &err);
	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// the program isn't needed anymore, release it
	clReleaseProgram(program);

	// create command queue
	queue = clCreateCommandQueue(context, device, 0, &err);
	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// create buffers (the size of the bit matrix)
	size = size_update * BIT_VEC_WIDTH(size_subscr) ;
	vec = clCreateBuffer(context, CL_MEM_READ_WRITE, size * sizeof(bitvec_elem), NULL, &err);
	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);
	vec2 = clCreateBuffer(context, CL_MEM_READ_ONLY, size * sizeof(bitvec_elem), NULL, &err);
	if (err != CL_SUCCESS)
		return set_error(err_opencl, __FILE__, __FUNCTION__, __LINE__);

	// the context isn't needed anymore, release it
	clReleaseContext(context);

	// set buffers as kernel arguments
	// kernel_not
	// __global const BITVECTOR_T *vec
	clSetKernelArg(kernel_not, 0, sizeof(cl_mem), &vec);
	// const uint size
	clSetKernelArg(kernel_not, 1, sizeof(_UINT), &size);
	
	// kernel_or
	// __global const BITVECTOR_T *vec
	clSetKernelArg(kernel_or, 0, sizeof(cl_mem), &vec);
	// __global const BITVECTOR_T *mask
	clSetKernelArg(kernel_or, 1, sizeof(cl_mem), &vec2);
	// const uint size
	clSetKernelArg(kernel_or, 2, sizeof(_UINT), &size);

	return err_none;
}


#ifndef __NOFREE
/** \brief Function that releases all the resources related to OpenCL.

\remarks Not sure if it can be ignored by __NOFREE. It seems so by multiple executions, but no tests have been done on it out of VS2010 (so behavior can be different in production).
*/
void cl_free()
{
	clReleaseMemObject(vec);
	clReleaseMemObject(vec2);
	clReleaseKernel(kernel_not);
	clReleaseKernel(kernel_or);
	clReleaseCommandQueue(queue);
}
#endif // __NOFREE
