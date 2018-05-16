/************************************************************
  Copyright (C) 2017-2018, �׵籩��. All right reserved.
  ProjectName:	��ʱͣ�� - ��� EasyX �� png ͼƬ֧��
  FileName:		EasyXPng.h
  Author:		�׵籩��
  Version:		1.0.0.1
  Date:			2018/02/28 13:50:00
  Description:	EasyXPng ͷ�ļ�
  History:
	<Author>	<time>		<version>		<desc>
	�׵籩��		18/02/28	1.0.0.1			����ͷ�ļ�
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