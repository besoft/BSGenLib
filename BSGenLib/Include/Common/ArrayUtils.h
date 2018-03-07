/* ${FILENAME}
** ${LASTMODIFIED}
**
** ${COPYRIGHT}
** ${LICENCE}
**
** DESCRIPTION: Helper class for array manipulation
*/

#ifndef BES_ARRAY_UTILS_H
#define BES_ARRAY_UTILS_H

template< typename T >
class CArrayUtils
{
public:

	/// <summary>Gets the minimum of an array.</summary>	
	/// <param name="pInData">Data to be inspected.</param>
	/// <param name="nCount">Number of elements in the array.</param>	
	/// <returns>The minimum.</returns>
	static T GetMin(const T* pInData, int nCount);

	/// <summary>Gets the maximum of an array.</summary>	
	/// <param name="pInData">Data to be inspected.</param>
	/// <param name="nCount">Number of elements in the array.</param>	
	/// <returns>The maximum.</returns>
	static T GetMax(const T* pInData, int nCount);

	/// <summary>Gets the minimum and maximum of an array.</summary>
	/// <param name="pInData">Data to be inspected.</param>
	/// <param name="nCount"> Number of elements in the array.</param>
	/// <param name="tmin">[out]the minimum.</param>
	/// <param name="tmax">[out]the maximum.</param>
	static void GetMinMax(const T* pInData, int nCount, T& tmin, T& tmax);
};

/// <summary>Gets the minimum of an array.</summary>	
/// <param name="pInData">Data to be inspected.</param>
/// <param name="nCount">Number of elements in the array.</param>	
/// <returns>The minimum.</returns>
template< typename T >
/*static*/ T CArrayUtils< T >::GetMin(const T* pInData, int nCount)
{
	T tmin = pInData[0];
	for (int i = 1; i < nCount; i++)
	{
		if (pInData[i] < tmin)
			tmin = pInData[i];		
	}

	return tmin;
}

/// <summary>Gets the maximum of an array.</summary>	
/// <param name="pInData">Data to be inspected.</param>
/// <param name="nCount">Number of elements in the array.</param>	
/// <returns>The maximum.</returns>
template< typename T >
/*static*/ T CArrayUtils< T >::GetMax(const T* pInData, int nCount)
{
	T tmax = pInData[0];
	for (int i = 1; i < nCount; i++)
	{
		if (pInData[i] > tmax)
			tmax = pInData[i];		
	}

	return tmax;
}

/// <summary>Gets the minimum and maximum of an array.</summary>
/// <param name="pInData">Data to be inspected.</param>
/// <param name="nCount"> Number of elements in the array.</param>
/// <param name="tmin">[out]the minimum.</param>
/// <param name="tmax">[out]the maximum.</param>
template< typename T >
/*static*/ void CArrayUtils< T >::GetMinMax(const T* pInData, int nCount, T& tmin, T& tmax)
{
	tmin = tmax = pInData[0];
	for (int i = 1; i < nCount; i++)
	{
		if (pInData[i] < tmin)
			tmin = pInData[i];

		if (pInData[i] > tmax)
			tmax = pInData[i];		
	}	
}


#endif		//BES_ARRAY_UTILS_H