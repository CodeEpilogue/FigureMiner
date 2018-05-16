/************************************************************
  Copyright (C) 2017-2018, 雷电暴雨. All right reserved.
  ProjectName:	限时停靠 - 针对 EasyX 的 png 图片支持
  FileName:		ImageEx.h
  Author:		雷电暴雨
  Version:		1.0.0.1
  Date:			2018/02/28 13:50:00
  Description:	ImageEx 头文件 (EasyX IMAGE 类型 Gdi+ 版)
  History:
	<Author>	<time>		<version>		<desc>
	雷电暴雨		18/02/28	1.0.0.1			建立头文件
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
