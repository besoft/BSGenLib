#pragma once

class CMemUtils
{
//public:
//	typedef struct MEMALLOC_STATISTICS
//	{		
//		UINT nPeekTotalMem;
//		UINT nPeekLargestMem;
//	}
//	MEMALLOC_STATISTICS;
//protected:
//	static MEMALLOC_STATISTICS m_Stats;
//	
public:
//	//monitors memory consimption
//	static void MonitorConsumptionStart(bool bResetStatistics = true);
//	static MEMALLOC_STATISTICS* MonitorConsumptionEnd();

	//returns the total number of memory 
	//currently allocated by the application
	static UINT GetMemoryConsumption();

	//returns the largest available block
	static UINT GetLargestFreeSpace();
};
