#include "StdAfx.h"
#ifndef __BSGEN_PUREWIN
#include <gdiplus.h>

using namespace Gdiplus;
/*static*/ CImageUtils CImageUtils::m_thePNGUtils;
/*static*/ ULONG_PTR CImageUtils::m_GDIPlus = NULL;

const CLSID* CImageUtils::ImageFormat::GetImageFormatCLSID()
{
	const static CLSID encoderClsid[] = {
		//PNG
		{0x557CF406, 0x1A04, 0x11D3, {0x9A, 0x73,0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E}},

		//BMP		
		{0x557CF400, 0x1A04, 0x11D3, {0x9A, 0x73,0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E}},

		//JPEG
		{0x557CF401, 0x1A04, 0x11D3, {0x9A, 0x73,0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E}},

		//TIFF
		{0x557CF405, 0x1A04, 0x11D3, {0x9A, 0x73,0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E}},
	};

	return &encoderClsid[m_nValue];
}


CImageUtils::CImageUtils()
{
	GdiplusStartupInput gdiplusStartupInput;   
	GdiplusStartup(&m_GDIPlus, &gdiplusStartupInput, NULL);
}

CImageUtils::~CImageUtils()
{
	GdiplusShutdown(m_GDIPlus);
}


//loads ARGB data from BMP/PNG/JPEG, returns false, if the file cannot be loaded
/*static*/ bool CImageUtils::LoadImage(LPCTSTR lpszPathName, SIZE* pSize, Pixel32** pData)
{
	USES_CONVERSION;

	if (::GetFileAttributes(lpszPathName) == (UINT)-1)
		return false;

	Bitmap* bmp = new Bitmap(T2W(const_cast<LPTSTR>(lpszPathName)));
	if (bmp->GetLastStatus() != Gdiplus::Ok) 
	{
		delete bmp;
		return false;
	}

	BitmapData* bitmapData = new BitmapData();
	
	Rect rect(0, 0, pSize->cx = bmp->GetWidth(), pSize->cy = bmp->GetHeight());
	bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, bitmapData);

	UINT nCount = rect.Width*rect.Height;	
	BYTE* pixels = (BYTE*)bitmapData->Scan0;

	*pData = new Pixel32[nCount];
	
	UINT nSrcIndex = 0, nDstIndex = 0;
	for (UINT y = 0; y < bitmapData->Height; y++) 
	{
		for (UINT x = 0; x < bitmapData->Width; x++) {
			//(*pData)[nDstIndex++].ARGB = *((UINT*)&pixels[nSrcIndex += 4]);
			(*pData)[nDstIndex].ARGB = *((UINT*)&pixels[nSrcIndex]);
			nDstIndex++; nSrcIndex += 4;
		}

		nSrcIndex += bitmapData->Stride - bitmapData->Width * 4; //4 => 32 bits
	}

	bmp->UnlockBits(bitmapData);
	delete bitmapData;
	delete bmp;	
	return true;
}


//loads data from BMP/PNG/JPEG, returns false, if the file cannot be loaded
/*static*/ bool CImageUtils::LoadImage(LPCTSTR lpszPathName, SIZE* pSize, BYTE** pDataR, 
		BYTE** pDataG, BYTE** pDataB)
{
	Pixel32* pPixels = NULL; 
	if (!LoadImage(lpszPathName, pSize, &pPixels))
		return false;

	int nCount = pSize->cx * pSize->cy;
	*pDataR = new BYTE[nCount];
	*pDataG = new BYTE[nCount];
	*pDataB = new BYTE[nCount];

	for (int i = 0; i < nCount; i++)
	{
		(*pDataR)[i] = pPixels[i].R;
		(*pDataG)[i] = pPixels[i].G;
		(*pDataB)[i] = pPixels[i].B;
	}

	delete[] pPixels;
	return true;
}

//loads data from BMP/PNG/JPEG, returns false, if the file cannot be loaded
/*static*/ bool CImageUtils::LoadImage(LPCTSTR lpszPathName, SIZE* pSize, BYTE** pData)
{			
	Pixel32* pPixels = NULL; 
	if (!LoadImage(lpszPathName, pSize, &pPixels))
		return false;

	int nCount = pSize->cx * pSize->cy;
	*pData = new BYTE[nCount];

	for (int i = 0; i < nCount; i++) {
		(*pData)[i] = pPixels[i].R;		
	}

	delete[] pPixels;
	return true;
}

//saves ARGB data into BMP/PNG/JPEG, returns false, if the file couldn't be created
/*static*/ bool CImageUtils::SaveImage(LPCTSTR lpszPathName, ImageFormat format, SIZE Size, Pixel32* pData)
{
	Bitmap* bmp = new Bitmap(Size.cx, Size.cy, PixelFormat32bppARGB);	
	BitmapData* bitmapData = new BitmapData();
	
	Rect rect(0, 0, bmp->GetWidth(), bmp->GetHeight());
	bmp->LockBits(&rect, ImageLockModeWrite, PixelFormat32bppARGB, bitmapData);

	UINT nCount = rect.Width*rect.Height;
	BYTE* pixels = (BYTE*)bitmapData->Scan0;
	
	UINT nSrcIndex = 0, nDstIndex = 0;
	for (UINT y = 0; y < bitmapData->Height; y++) 
	{
		for (UINT x = 0; x < bitmapData->Width; x++) {
			*((UINT*)&pixels[nDstIndex]) = pData[nSrcIndex].ARGB;			
			nDstIndex += 4; nSrcIndex++;
		}

		nDstIndex += bitmapData->Stride - bitmapData->Width * 4; //4 => 32 bits
	}

	bmp->UnlockBits(bitmapData);
	delete bitmapData;
		
	USES_CONVERSION;
	bool bRet = bmp->Save(T2W(const_cast<LPTSTR>(lpszPathName)), format.GetImageFormatCLSID()) == 0;
	delete bmp;	
	return bRet;
}



//saves RGB data into BMP/PNG/JPEG, returns false, if the file couldn't be created
/*static*/ bool CImageUtils::SaveImage(LPCTSTR lpszPathName, ImageFormat format, SIZE Size, BYTE* pDataR, 
		BYTE* pDataG, BYTE* pDataB)
{
	Bitmap* bmp = new Bitmap(Size.cx, Size.cy, PixelFormat24bppRGB);	
	BitmapData* bitmapData = new BitmapData();
	
	Rect rect(0, 0, bmp->GetWidth(), bmp->GetHeight());
	bmp->LockBits(&rect, ImageLockModeWrite, PixelFormat24bppRGB, bitmapData);

	UINT nCount = rect.Width*rect.Height;
	BYTE* pixels = (BYTE*)bitmapData->Scan0;
	
	UINT nSrcIndex = 0, nDstIndex = 0;
	for (UINT y = 0; y < bitmapData->Height; y++) 
	{
		for (UINT x = 0; x < bitmapData->Width; x++) {

			pixels[nDstIndex++] = pDataR[nSrcIndex];
			pixels[nDstIndex++] = pDataG[nSrcIndex];
			pixels[nDstIndex++] = pDataB[nSrcIndex++];
		}

		nDstIndex += bitmapData->Stride - bitmapData->Width * 3; //3 => 24 bits
	}

	bmp->UnlockBits(bitmapData);
	delete bitmapData;
	

	USES_CONVERSION;
	bool bRet = bmp->Save(T2W(const_cast<LPTSTR>(lpszPathName)),  format.GetImageFormatCLSID()) == 0;
	delete bmp;	
	return bRet;
}
#endif