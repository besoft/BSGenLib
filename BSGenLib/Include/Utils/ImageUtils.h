#pragma once
#ifndef __BSGEN_PUREWIN
class CImageUtils
{
public:
	typedef struct Pixel32
	{
		union 
		{
			UINT ARGB;
			struct 
			{
				BYTE B;
				BYTE G;
				BYTE R;
				BYTE A;
			};
		};
	} Pixel32;

	typedef struct ImageFormat 
	{
		DECLARE_ENUM_STRUCT(ImageFormat);
		enum {
			PNG = 0,
			BMP = 1,
			JPEG = 2,
			TIFF = 3,
		};

		const CLSID* GetImageFormatCLSID();
	} ImageFormat;

protected:
	static CImageUtils m_thePNGUtils;
	static ULONG_PTR m_GDIPlus;

protected:
	CImageUtils();
	~CImageUtils();

public:
#pragma region LoadImage
	//loads greyscale data from BMP/PNG/JPEG, returns false, if the file cannot be loaded
	static bool LoadImage(LPCTSTR lpszPathName, SIZE* pSize, BYTE** pData);

	//loads ARGB data from BMP/PNG/JPEG, returns false, if the file cannot be loaded
	static bool LoadImage(LPCTSTR lpszPathName, SIZE* pSize, BYTE** pDataR, 
		BYTE** pDataG, BYTE** pDataB);

	//loads ARGB data from BMP/PNG/JPEG, returns false, if the file cannot be loaded
	static bool LoadImage(LPCTSTR lpszPathName, SIZE* pSize, Pixel32** pData);
#pragma endregion
	
#pragma region SaveImage
	//saves greyscale data into BMP/PNG/JPEG, returns false, if the file couldn't be created
	inline static bool SaveImage(LPCTSTR lpszPathName, ImageFormat format, SIZE Size, BYTE* pData) {
		return SaveImage(lpszPathName, format, Size, pData, pData, pData);
	}

	//saves RGB data into BMP/PNG/JPEG, returns false, if the file couldn't be created
	static bool SaveImage(LPCTSTR lpszPathName, ImageFormat format, SIZE Size, BYTE* pDataR, 
		BYTE* pDataG, BYTE* pDataB);

	//saves ARGB data into BMP/PNG/JPEG, returns false, if the file couldn't be created
	static bool SaveImage(LPCTSTR lpszPathName, ImageFormat format, SIZE Size, Pixel32* pData);


	//saves greyscale data into PNG, returns false, if the file cannot be stored
	inline static bool SavePNG(LPCTSTR lpszPathName, SIZE Size, BYTE* pData) {
		return SaveImage(lpszPathName, ImageFormat::PNG, Size, pData);
	}
	
	//saves ARGB data into PNG, returns false, if the file cannot be stored
	inline static bool SavePNG(LPCTSTR lpszPathName, SIZE Size, BYTE* pDataR, 
		BYTE* pDataG, BYTE* pDataB) {
		return SaveImage(lpszPathName, ImageFormat::PNG, Size, pDataR, pDataG, pDataB);
	}

	//saves ARGB data into PNG, returns false, if the file cannot be stored
	inline static bool SavePNG(LPCTSTR lpszPathName, SIZE Size, Pixel32* pData) {
		return SaveImage(lpszPathName, ImageFormat::PNG, Size, pData);
	}

	//saves greyscale data into BMP, returns false, if the file cannot be stored
	inline static bool SaveBMP(LPCTSTR lpszPathName, SIZE Size, BYTE* pData) {
		return SaveImage(lpszPathName, ImageFormat::BMP, Size, pData);
	}
	
	//saves ARGB data into BMP, returns false, if the file cannot be stored
	inline static bool SaveBMP(LPCTSTR lpszPathName, SIZE Size, BYTE* pDataR, 
		BYTE* pDataG, BYTE* pDataB) {
		return SaveImage(lpszPathName, ImageFormat::BMP, Size, pDataR, pDataG, pDataB);
	}

	//saves ARGB data into BMP, returns false, if the file cannot be stored
	inline static bool SaveBMP(LPCTSTR lpszPathName, SIZE Size, Pixel32* pData) {
		return SaveImage(lpszPathName, ImageFormat::BMP, Size, pData);
	}

	//saves greyscale data into JPEG, returns false, if the file cannot be stored
	inline static bool SaveJPEG(LPCTSTR lpszPathName, SIZE Size, BYTE* pData) {
		return SaveImage(lpszPathName, ImageFormat::JPEG, Size, pData);
	}
	
	//saves ARGB data into JPEG, returns false, if the file cannot be stored
	inline static bool SaveJPEG(LPCTSTR lpszPathName, SIZE Size, BYTE* pDataR, 
		BYTE* pDataG, BYTE* pDataB) {
		return SaveImage(lpszPathName, ImageFormat::JPEG, Size, pDataR, pDataG, pDataB);
	}

	//saves ARGB data into JPEG, returns false, if the file cannot be stored
	inline static bool SaveJPEG(LPCTSTR lpszPathName, SIZE Size, Pixel32* pData) {
		return SaveImage(lpszPathName, ImageFormat::JPEG, Size, pData);
	}
#pragma endregion

};

#pragma comment( lib, "gdiplus.lib" ) 
#endif
