/************************************************************
  Copyright (C) 2017-2018, �׵籩��. All right reserved.
  ProjectName:	��ʱͣ�� - ��� EasyX �� png ͼƬ֧��
  FileName:		EasyXPng.cpp
  Author:		�׵籩��
  Version:		1.0.0.1
  Date:			2018/02/28 13:50:00
  Description:	EasyXPng Դ�ļ�
  History:
	<Author>	<time>		<version>		<desc>
	�׵籩��		18/02/28	1.0.0.1			����Դ�ļ�
 ***********************************************************/

#include "EasyXPng.h"

using namespace Gdiplus;
using namespace std;

static ULONG_PTR gdiplusToken;
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;

#define WCHAR_BUFFER_MAX 1024
static wchar_t wcharBuffer[WCHAR_BUFFER_MAX];

/**************************************
 charToWChar ASCII �ַ�ת���ַ�
 ������
	const char * ss ԭʼ�ַ���
 ����ֵ��
	wchar_t * ���ַ���
 **************************************/
wchar_t * charToWChar(const char * ss) {
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, ss, -1, NULL, 0 );
	if (nLen == 0 || (nLen * sizeof(wchar_t)) >= WCHAR_BUFFER_MAX) {
		return NULL;
	}

	int ret = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, ss, -1, wcharBuffer, nLen );
	return wcharBuffer;
}

/**************************************
 ResizeBitmap ����λͼ (GDI+)
 ������
	Gdiplus::Bitmap * bmpSrc ԭʼλͼָ��
	int destWidth Ŀ����
	int destHeight Ŀ��߶�
	bool keepAspect �Ƿ񱣳ֿ�߱�
 ����ֵ��
	Gdiplus::Bitmap * ��ת��λͼָ��
 **************************************/
Gdiplus::Bitmap * ResizeBitmap(Gdiplus::Bitmap * bmpSrc, int destWidth, int destHeight, bool keepAspect = false) { 
    // ȡ��ԴͼƬ��Ⱥ͸߶�
    int srcWidth = bmpSrc->GetWidth();
    int srcHeight = bmpSrc->GetHeight();
    // ����������������ű��� 
    float scaleH = (float)destWidth / srcWidth;
    float scaleV = (float)destHeight / srcHeight;
    // �����Ҫ���ֿ�߱ȣ�����������ͳһ���ý�С�����ű���
    if (keepAspect) {
        if (scaleH > scaleV) {
            scaleH = scaleV;
        }
        else {
            scaleV = scaleH;
        }
    }
    // ����Ŀ���ȸ߶�
    destWidth = (int)(srcWidth * scaleH);
    destHeight = (int)(srcHeight * scaleV);
    // ����Ŀ�� Bitmap
    Gdiplus::Bitmap * bmpDest = new Gdiplus::Bitmap(destWidth, destHeight, PixelFormat32bppARGB);
    Gdiplus::Graphics graphic(bmpDest);
    // ���ò�ֵ�㷨
    graphic.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    // ��Դͼ����Ƶ�Ŀ��ͼ����
    graphic.DrawImage(bmpSrc, Gdiplus::Rect(0, 0, destWidth, destHeight), 
		0, 0, srcWidth, srcHeight, Gdiplus::UnitPixel);
    graphic.Flush();
    return bmpDest;
}

/**************************************
 initgraphEx ��ʼ�� GDI ��ͼ�豸
 ��������
 ����ֵ��
	Gdiplus::Status GDI ͼ���豸״̬
 **************************************/
Gdiplus::Status initgraphEx()
{
	return Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

/**************************************
 closegraphEx �ر� GDI ��ͼ�豸
 ��������
 ����ֵ����
 **************************************/
void closegraphEx()
{
	if (gdiplusToken) {
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
	closegraph();
}

/**************************************
 loadimage �� EasyX �� loadimage ��������
 ������
	CImageEx * image ͼ������ָ��
	LPCTSTR pImgFile ͼ���ļ�·��
	int nWidth Ĭ�Ͽ��
	int nHeight Ĭ�ϸ߶�
 ����ֵ����
 **************************************/
void loadimage(CImageEx * image, LPCTSTR pImgFile, int nWidth, int nHeight) {
	if(image == NULL || image->pngImage != NULL)
		return;

	wchar_t * wFileName = charToWChar(pImgFile);
	//wchar_t * wFileName = (wchar_t *)pImgFile;
	image->pngImage = Gdiplus::Bitmap::FromFile(wFileName);
	
	if(nWidth != 0 && nHeight != 0) {
		Bitmap * backup = image->pngImage;
		image->pngImage = ResizeBitmap(image->pngImage, nWidth, nHeight);
		delete backup;
	}
}

/**************************************
 putimage �� EasyX �� putimage ��������
 ������
	int dstX ������
	int dstY ������
	CImageEx * image ͼ������ָ��
 ����ֵ����
 **************************************/
void putimage(int dstX, int dstY, CImageEx *image) {
	Gdiplus::Graphics * graphic = Gdiplus::Graphics::FromHDC(GetImageHDC());
	graphic->DrawImage(image->pngImage, dstX, dstY, image->getWidth(), image->getHeight());
	delete graphic;
}

void rotateimage(CImageEx *dstimg, CImageEx *srcimg, float angle)
{
	if (dstimg == NULL || srcimg == NULL) {
		return;
	}
	
	// ���ԭʼͼƬ�Ĵ�С
	int srcWidth = srcimg->getWidth();
    int srcHeight = srcimg->getHeight();

	// �����ԭʼͼƬ�Խ��ߵľ��� ��ΪĿ��ͼƬ�ĳ��Ϳ�
	int dstWidth = (int)(sqrt(srcWidth * srcWidth + srcHeight * srcHeight) + 0.5);
    int dstHeight = dstWidth;
	
	if (dstimg->pngImage != NULL) {
		delete dstimg->pngImage;
	}

	// ����ԭʼͼƬ�Խ��߳��� ����Ŀ��ͼƬ���Ϳ��������Ƴ�����ת��ͼƬ���ᱻ��������
	dstimg->pngImage = new Gdiplus::Bitmap(dstWidth, dstHeight, PixelFormat32bppARGB);

    Gdiplus::Graphics graphic(dstimg->pngImage);
	
	// Pen * pen = new Pen(Color::Yellow); //������ ������
	// graphic.DrawRectangle(pen, 0, 0, dstWidth - 1, dstHeight - 1); //������ ������ ���Ƴ���ת��ͼƬ�ı߽�

	Matrix m; // ����任�õľ������
	PointF cp(REAL(srcWidth/2.0f), REAL(srcHeight/2.0f)); //center point ��ת������
	PointF mp(REAL((dstWidth - srcWidth)/2.0f), (dstHeight - srcHeight)/2.0f);

	// ע�� ����任��˳���Ƿ��� Ҳ����˵ ��������Ч���൱������ת ���ƶ�
	m.Translate(mp.X, mp.Y); // Ч��Ϊ �Ƚ�ԭʼͼƬ���ĵ��ƶ���Ŀ��ͼƬ������
	m.RotateAt(angle, cp); // Ч��Ϊ ����Ŀ��ͼƬ�����ĵ������ת
	graphic.SetTransform(&m); // ���þ���任�õľ��������

	// ��ͼ ͬʱ���о���任
	graphic.DrawImage(srcimg->pngImage, Gdiplus::Rect(0, 0, srcWidth, srcHeight), 
		0, 0 , srcWidth, srcHeight, Gdiplus::UnitPixel);
	// graphic.DrawRectangle(pen, 0, 0, srcWidth - 1, srcHeight - 1); // ������ ������ ���Ƴ�ԭʼͼƬ��Ŀ��ͼ���е�λ��

	// delete pen;
}