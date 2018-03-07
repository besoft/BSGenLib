#include "StdAfx.h"
#include "../../Include/Common/MemUtils.h"

#define LIMIT_PTR  (LPVOID)((INT_PTR)(0x80000000))

//returns the total number of memory currently allocated by the application
UINT CMemUtils::GetMemoryConsumption()
{
	MEMORY_BASIC_INFORMATION ssMemInfo;
	memset(&ssMemInfo, 0, sizeof(ssMemInfo));

	UINT nTotalMem = 0;
	do
	{
		VirtualQueryEx(::GetCurrentProcess(), ssMemInfo.BaseAddress,&ssMemInfo,  sizeof(ssMemInfo));		
		if (ssMemInfo.State != MEM_FREE)
			nTotalMem += (UINT)ssMemInfo.RegionSize;

		ssMemInfo.BaseAddress = ((BYTE*)ssMemInfo.BaseAddress) + ssMemInfo.RegionSize;
	} while (ssMemInfo.BaseAddress < LIMIT_PTR);

	return nTotalMem;
}

//returns the largest available block
UINT CMemUtils::GetLargestFreeSpace()
{
	MEMORY_BASIC_INFORMATION ssMemInfo;
	memset(&ssMemInfo, 0, sizeof(ssMemInfo));

	UINT nTotalMem = 0;
	do
	{
		VirtualQueryEx(::GetCurrentProcess(), ssMemInfo.BaseAddress,&ssMemInfo,  sizeof(ssMemInfo));		
		if (ssMemInfo.State == MEM_FREE)
		{
			if (ssMemInfo.RegionSize > nTotalMem)
				nTotalMem = (UINT)ssMemInfo.RegionSize;
		}

		ssMemInfo.BaseAddress = ((BYTE*)ssMemInfo.BaseAddress) + ssMemInfo.RegionSize;
	} while (ssMemInfo.BaseAddress < LIMIT_PTR);

	return nTotalMem;
}