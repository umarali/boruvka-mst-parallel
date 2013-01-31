/**********************************************************************************
 * Copyright (c) 2008-2009 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 **********************************************************************************/

/* $Revision$ on $Date$ */

#ifndef __OPENCL_CL_D3D9_H
#define __OPENCL_CL_D3D9_H

#include <CL/cl_platform.h>
#include <d3d9.h>
#include <dxvahd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* cl_intel_dx9_media_sharing extension    */
#define cl_intel_dx9_media_sharing 1

typedef cl_uint cl_dx9_device_source_intel;
typedef cl_uint cl_dx9_device_set_intel;

/******************************************************************************/

// Error Codes
#define CL_INVALID_DX9_DEVICE_INTEL                   -1010
#define CL_INVALID_DX9_RESOURCE_INTEL                 -1011
#define CL_DX9_RESOURCE_ALREADY_ACQUIRED_INTEL        -1012
#define CL_DX9_RESOURCE_NOT_ACQUIRED_INTEL            -1013

// cl_dx9_device_source_intel
#define CL_D3D9_DEVICE_INTEL                          0x4022
#define CL_D3D9EX_DEVICE_INTEL                        0x4070
#define CL_DXVA_DEVICE_INTEL                          0x4071

// cl_dx9_device_set_intel
#define CL_PREFERRED_DEVICES_FOR_DX9_INTEL            0x4024
#define CL_ALL_DEVICES_FOR_DX9_INTEL                  0x4025

// cl_context_info
#define CL_CONTEXT_D3D9_DEVICE_INTEL                  0x4026
#define CL_CONTEXT_D3D9EX_DEVICE_INTEL                0x4072
#define CL_CONTEXT_DXVA_DEVICE_INTEL                  0x4073

// cl_mem_info
#define CL_MEM_DX9_RESOURCE_INTEL                     0x4027
#define CL_MEM_DX9_SHARED_HANDLE_INTEL                0x4074

// cl_image_info
#define CL_IMAGE_DX9_PLANE_INTEL                      0x4075
#define CL_IMAGE_D3D9_LEVEL_INTEL                     0x4080
#define CL_IMAGE_D3D9_FACE_INTEL                      0x4081

// cl_command_type
#define CL_COMMAND_ACQUIRE_DX9_OBJECTS_INTEL          0x402A
#define CL_COMMAND_RELEASE_DX9_OBJECTS_INTEL          0x402B
//packed YUV channel order
#define CL_YUYV_INTEL                                 0x4076
#define CL_UYVY_INTEL                                 0x4077
#define CL_YVYU_INTEL                                 0x4078
#define CL_VYUY_INTEL                                 0x4079

/******************************************************************************/

/* cl_context_properties            */
#define CL_CONTEXT_D3D9_DEVICE 0x1085

extern CL_API_ENTRY cl_int CL_API_CALL
clGetDeviceIDsFromDX9INTEL( 
    cl_platform_id              /* platform */,
    cl_dx9_device_source_intel  /* d3d_device_source */,
    void*                       /* dx9_object */,
    cl_dx9_device_set_intel     /* d3d_device_set */,
    cl_uint                     /* num_entries */, 
    cl_device_id*               /* devices */, 
    cl_uint*                    /* num_devices */ );

extern CL_API_ENTRY cl_mem CL_API_CALL
clCreateFromDX9MediaSurfaceINTEL( 
    cl_context          /* context */,
    cl_mem_flags        /* flags */,
    IDirect3DSurface9*  /* resource */,
    HANDLE              /* sharedHandle */,
    UINT                /* plane */,
    cl_int*             /* errcode_ret */ );

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueAcquireDX9ObjectsINTEL( 
    cl_command_queue /* command_queue */,
    cl_uint          /* num_objects */,
    const cl_mem*    /* mem_objects */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event*  /* event_wait_list */,
    cl_event*        /* ocl_event */ );

extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueReleaseDX9ObjectsINTEL( 
    cl_command_queue /* command_queue */,
    cl_uint          /* num_objects */,
    const cl_mem *   /* mem_objects */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event * /* event_wait_list */,
    cl_event *       /* ocl_event */ );

#ifdef __cplusplus
}
#endif

#endif  /* __OPENCL_CL_D3D9_H   */

