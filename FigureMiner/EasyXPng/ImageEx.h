/************************************************************
  Copyright (C) 2017-2018, �׵籩��. All right reserved.
  ProjectName:	��ʱͣ�� - ��� EasyX �� png ͼƬ֧��
  FileName:		ImageEx.h
  Author:		�׵籩��
  Version:		1.0.0.1
  Date:			2018/02/28 13:50:00
  Description:	ImageEx ͷ�ļ� (EasyX IMAGE ���� Gdi+ ��)
  History:
	<Author>	<time>		<version>		<desc>
	�׵籩��		18/02/28	1.0.0.1			����ͷ�ļ�
 ***********************************************************/

#ifndef IMAGEEX_H
#define IMAGEEX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <easyx.h>
#include <gdiplus.h>
class Gdiplus::Bitmap;

class CImageEx
{
public:
	int getHeight();
	int getWidth();
	CImageEx();
	virtual ~CImageEx();
	Gdiplus::Bitmap * pngImage;
	float angle;
	void CImageEx::Resize(int _width, int _height);
};

typedef CImageEx IMAGE_PNG;

#endif
