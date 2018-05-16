/************************************************************
  Copyright (C) 2017-2018, 雷电暴雨. All right reserved.
  ProjectName:	限时停靠 - 针对 EasyX 的 png 图片支持
  FileName:		EasyXPng.h
  Author:		雷电暴雨
  Version:		1.0.0.1
  Date:			2018/02/28 13:50:00
  Description:	EasyXPng 头文件
  History:
	<Author>	<time>		<version>		<desc>
	雷电暴雨		18/02/28	1.0.0.1			建立头文件
 ***********************************************************/

#ifndef EASYXPNG_H
#define EASYXPNG_H

#include <iostream>
#include <math.h>
#include <easyx.h>
#include "ImageEx.h"

#pragma comment(lib, "Gdiplus.lib")

wchar_t * charToWChar(const char * ss);
Gdiplus::Status initgraphEx();
void closegraphEx();
void loadimage(CImageEx * image, LPCTSTR pImgFile, int nWidth = 0, int nHeight = 0);
//void loadimage(CImageEx * image, LPCTSTR pResType, LPCTSTR pResName, int nWidth = 0, int nHeight = 0);
void putimage(int dstX, int dstY, CImageEx *image);
void rotateimage(CImageEx *dstimg, CImageEx *srcimg, float angle);
#endif