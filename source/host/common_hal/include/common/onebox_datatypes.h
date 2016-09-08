/**
 * @file onebox_datatypes.h
 * @author 
 * @version 1.0
 *
 * @section LICENSE
 *
 * This software embodies materials and concepts that are confidential to Redpine
 * Signals and is made available solely pursuant to the terms of a written license
 * agreement with Redpine Signals
 *
 * @section DESCRIPTION
 *
 * This file contians the data type defines  .
 */

#ifndef __H_ONEBOX_DATATYPES_H__
#define __H_ONEBOX_DATATYPES_H__

#include <linux/kernel.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
	typedef unsigned char UCHAR,WCHAR, uint8, * PUCHAR;
	typedef unsigned short uint16;
	typedef unsigned int uint32;
	typedef unsigned long uint64;

	typedef char int8, *PCHAR;
	typedef short int16;
	typedef int int32;

	typedef void VOID, * PVOID;
	typedef bool BOOLEAN;
#elif((LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18))&& \
   (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,23)))
	typedef unsigned char WCHAR,uint8;
	typedef unsigned short uint16;
	typedef unsigned int uint32;

	typedef char int8;
	typedef int int32;
	typedef unsigned long uint64;

	typedef void VOID;
#endif
#endif
