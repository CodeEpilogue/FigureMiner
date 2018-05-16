/************************************************************
  Copyright (C) 2017-2018, �׵籩��. All right reserved.
  ProjectName:	��ʱͣ�� - ��� EasyX �� png ͼƬ֧��
  FileName:		ImageEx.cpp
  Author:		�׵籩��
  Version:		1.0.0.1
  Date:			2018/02/28 13:52:00
  Description:	ImageEx Դ�ļ� (EasyX IMAGE ���� Gdi+ ��)
  History:
	<Author>	<time>		<version>		<desc>
	�׵籩��		18/02/28	1.0.0.1			����Դ�ļ�
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