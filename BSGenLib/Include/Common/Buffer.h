#pragma once

template < typename TYPE, typename ARG_TYPE = const TYPE&>
class CBuffer
{
protected:
	TYPE* m_pBuffer;			//buffer
	UINT m_nBufferSize;			//total size of buffer (in elements)
	UINT m_nBufferLength;		//valid length of buffer (in elements)	
	bool m_bBufferDirty;		//true if some data has been modified

public:	
	CBuffer(UINT nBufferSize)
	{
		m_pBuffer = new TYPE[nBufferSize];
		m_nBufferSize = nBufferSize;
		m_nBufferLength = 0;
		m_bBufferDirty = false;
	}

	CBuffer(TYPE* pNewBuffer, UINT nBufferSize, UINT nBufferLength)
	{
		m_pBuffer = pNewBuffer;
		m_nBufferSize = nBufferSize;
		m_nBufferLength = nBufferLength;
		m_bBufferDirty = false;
	}

	~CBuffer()
	{
		delete[] m_pBuffer;
	}

public:		
	//adds new item into the buffer
	inline void Add(ARG_TYPE e);

	//copies data into the buffer, buffer must be large enough to handle incoming data
	inline void Append(const TYPE* pData, UINT nLength);

	//gets the data at given index
	inline ARG_TYPE GetAt(UINT nIndex);

	//gets the data at given index
	inline void SetAt(UINT nIndex, ARG_TYPE e);

	//resets buffer
	inline void ResetContent();


	//gets the buffer
	inline TYPE* GetBuffer() { return m_pBuffer; }

	//gets the size of buffer
	inline UINT GetSize() { return m_nBufferSize; }

	//gets the valid length of buffer
	inline UINT GetLength() { return m_nBufferLength; }

	//sets the valid length of buffer
	inline void SetLength(UINT nLength) { m_nBufferLength = nLength; }

	//returns true, if the buffer is empty
	inline bool IsEmpty() { return m_nBufferLength == 0; }

	//returns true, if the buffer is full
	inline bool IsFull() { return m_nBufferLength == m_nBufferSize; }

	//returns true, if the buffer has been modified
	inline bool IsDirty() { return m_bBufferDirty; }

	//modifies the dirty flag
	inline void SetDirty(bool bDirty = true) { m_bBufferDirty = bDirty; }	

	//indexers
	inline const TYPE& operator[](UINT nIndex) const { return GetAt(nIndex); }
	inline TYPE& operator[](UINT nIndex) { return const_cast<TYPE&>(GetAt(nIndex)); }
};

//adds new item into the buffer
template < typename TYPE, typename ARG_TYPE >
inline void CBuffer<TYPE, ARG_TYPE>::Add(ARG_TYPE e)
{
	_ASSERT(m_nBufferLength < m_nBufferSize);

	m_pBuffer[m_nBufferLength] = e;
	m_nBufferLength++;

	m_bBufferDirty = true;
}

//copies data into the buffer, buffer must be large enough to handle incoming data
template < typename TYPE, typename ARG_TYPE >
inline void CBuffer<TYPE, ARG_TYPE>::Append(const TYPE* pData, UINT nLength)
{
	_ASSERT(m_nBufferLength + nLength < m_nBufferSize);

	memcpy(&m_pBuffer[m_nBufferLength], pData, nLength * sizeof(TYPE));
	m_nBufferSize += nLength;

	m_bBufferDirty = true;
}

//gets the data at given index
template < typename TYPE, typename ARG_TYPE >
inline ARG_TYPE CBuffer<TYPE, ARG_TYPE>::GetAt(UINT nIndex)
{
	_ASSERT(nIndex < m_nBufferLength);

	return m_pBuffer[nIndex];
}

//gets the data at given index
template < typename TYPE, typename ARG_TYPE >
inline void CBuffer<TYPE, ARG_TYPE>::SetAt(UINT nIndex, ARG_TYPE e)
{
	_ASSERT(nIndex < m_nBufferLength);

	m_pBuffer[nIndex] = e;
	m_bBufferDirty = true;
}

//resets buffer
template < typename TYPE, typename ARG_TYPE >
inline void CBuffer<TYPE, ARG_TYPE>::ResetContent()
{
	m_nBufferLength = 0;
	m_bBufferDirty = false;
}