/************************************************************
  Copyright (C) 2017-2018, 雷电暴雨. All right reserved.
  ProjectName:	限时停靠 - 针对 EasyX 的 png 图片支持
  FileName:		ImageEx.cpp
  Author:		雷电暴雨
  Version:		1.0.0.1
  Date:			2018/02/28 13:52:00
  Description:	ImageEx 源文件 (EasyX IMAGE 类型 Gdi+ 版)
  History:
	<Author>	<time>		<version>		<desc>
	雷电暴雨		18/02/28	1.0.0.1			建立源文件
 ***********************************************************/

#include "ImageEx.h"

CImageEx::CImageEx()
{
	pngImage = NULL;
	angle = 0.0f;
	//hBmp = NULL;
}

CImageEx::~CImageEx()
{
	if(pngImage != NULL)
		delete pngImage;
	//if(hBmp)
	//	DeleteObject(hBmp);
}

int CImageEx::getWidth()
{
	return pngImage->GetWidth();
}

int CImageEx::getHeight()
{
	return pngImage->GetHeight();
}

void CImageEx::Resize(int _width, int _height) {
	int srcWidth = this->getWidth();
	int srcHeight = this->getHeight();
	Gdiplus::Graphics *gc;
	Gdiplus::Bitmap *temp;
	Gdiplus::Matrix m;

	temp = new Gdiplus::Bitmap(srcWidth, srcHeight, PixelFormat32bppARGB);

	gc = new Gdiplus::Graphics(temp);
	gc->DrawImage(this->pngImage, Gdiplus::Rect(0, 0, srcWidth, srcHeight));
	delete gc;

	this->pngImage = new Gdiplus::Bitmap(_width, _height, PixelFormat32bppARGB);
	gc = new Gdiplus::Graphics(this->pngImage);
	
	gc->DrawImage(temp, 0, 0, _width, _height);
	delete gc;
}