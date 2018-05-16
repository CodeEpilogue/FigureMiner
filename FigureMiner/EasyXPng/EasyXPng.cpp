/************************************************************
  Copyright (C) 2017-2018, 雷电暴雨. All right reserved.
  ProjectName:	限时停靠 - 针对 EasyX 的 png 图片支持
  FileName:		EasyXPng.cpp
  Author:		雷电暴雨
  Version:		1.0.0.1
  Date:			2018/02/28 13:50:00
  Description:	EasyXPng 源文件
  History:
	<Author>	<time>		<version>		<desc>
	雷电暴雨		18/02/28	1.0.0.1			建立源文件
 ***********************************************************/

#include "EasyXPng.h"

using namespace Gdiplus;
using namespace std;

static ULONG_PTR gdiplusToken;
static Gdiplus::GdiplusStartupInput gdiplusStartupInput;

#define WCHAR_BUFFER_MAX 1024
static wchar_t wcharBuffer[WCHAR_BUFFER_MAX];

/**************************************
 charToWChar ASCII 字符转宽字符
 参数：
	const char * ss 原始字符串
 返回值：
	wchar_t * 宽字符串
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
 ResizeBitmap 缩放位图 (GDI+)
 参数：
	Gdiplus::Bitmap * bmpSrc 原始位图指针
	int destWidth 目标宽度
	int destHeight 目标高度
	bool keepAspect 是否保持宽高比
 返回值：
	Gdiplus::Bitmap * 旋转后位图指针
 **************************************/
Gdiplus::Bitmap * ResizeBitmap(Gdiplus::Bitmap * bmpSrc, int destWidth, int destHeight, bool keepAspect = false) { 
    // 取得源图片宽度和高度
    int srcWidth = bmpSrc->GetWidth();
    int srcHeight = bmpSrc->GetHeight();
    // 计算横向和纵向的缩放比率 
    float scaleH = (float)destWidth / srcWidth;
    float scaleV = (float)destHeight / srcHeight;
    // 如果需要保持宽高比，则横向和纵向统一采用较小的缩放比率
    if (keepAspect) {
        if (scaleH > scaleV) {
            scaleH = scaleV;
        }
        else {
            scaleV = scaleH;
        }
    }
    // 计算目标宽度高度
    destWidth = (int)(srcWidth * scaleH);
    destHeight = (int)(srcHeight * scaleV);
    // 创建目标 Bitmap
    Gdiplus::Bitmap * bmpDest = new Gdiplus::Bitmap(destWidth, destHeight, PixelFormat32bppARGB);
    Gdiplus::Graphics graphic(bmpDest);
    // 设置插值算法
    graphic.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    // 将源图像绘制到目标图像上
    graphic.DrawImage(bmpSrc, Gdiplus::Rect(0, 0, destWidth, destHeight), 
		0, 0, srcWidth, srcHeight, Gdiplus::UnitPixel);
    graphic.Flush();
    return bmpDest;
}

/**************************************
 initgraphEx 初始化 GDI 绘图设备
 参数：无
 返回值：
	Gdiplus::Status GDI 图形设备状态
 **************************************/
Gdiplus::Status initgraphEx()
{
	return Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

/**************************************
 closegraphEx 关闭 GDI 绘图设备
 参数：无
 返回值：无
 **************************************/
void closegraphEx()
{
	if (gdiplusToken) {
		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
	closegraph();
}

/**************************************
 loadimage 对 EasyX 的 loadimage 函数重载
 参数：
	CImageEx * image 图像容器指针
	LPCTSTR pImgFile 图像文件路径
	int nWidth 默认宽度
	int nHeight 默认高度
 返回值：无
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
 putimage 对 EasyX 的 putimage 函数重载
 参数：
	int dstX 横坐标
	int dstY 纵坐标
	CImageEx * image 图像容器指针
 返回值：无
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
	
	// 获得原始图片的大小
	int srcWidth = srcimg->getWidth();
    int srcHeight = srcimg->getHeight();

	// 计算出原始图片对角线的距离 作为目标图片的长和宽
	int dstWidth = (int)(sqrt(srcWidth * srcWidth + srcHeight * srcHeight) + 0.5);
    int dstHeight = dstWidth;
	
	if (dstimg->pngImage != NULL) {
		delete dstimg->pngImage;
	}

	// 按照原始图片对角线长度 设置目标图片长和宽（这样绘制出的旋转后图片不会被剪辑掉）
	dstimg->pngImage = new Gdiplus::Bitmap(dstWidth, dstHeight, PixelFormat32bppARGB);

    Gdiplus::Graphics graphic(dstimg->pngImage);
	
	// Pen * pen = new Pen(Color::Yellow); //辅助线 测试用
	// graphic.DrawRectangle(pen, 0, 0, dstWidth - 1, dstHeight - 1); //辅助线 测试用 绘制出旋转后图片的边界

	Matrix m; // 矩阵变换用的矩阵变量
	PointF cp(REAL(srcWidth/2.0f), REAL(srcHeight/2.0f)); //center point 旋转的中心
	PointF mp(REAL((dstWidth - srcWidth)/2.0f), (dstHeight - srcHeight)/2.0f);

	// 注意 矩阵变换的顺序是反的 也就是说 下面代码的效果相当于先旋转 后移动
	m.Translate(mp.X, mp.Y); // 效果为 先将原始图片中心点移动到目标图片的中心
	m.RotateAt(angle, cp); // 效果为 再以目标图片的中心点进行旋转
	graphic.SetTransform(&m); // 设置矩阵变换用的矩阵变量。

	// 绘图 同时进行矩阵变换
	graphic.DrawImage(srcimg->pngImage, Gdiplus::Rect(0, 0, srcWidth, srcHeight), 
		0, 0 , srcWidth, srcHeight, Gdiplus::UnitPixel);
	// graphic.DrawRectangle(pen, 0, 0, srcWidth - 1, srcHeight - 1); // 辅助线 测试用 绘制出原始图片在目标图框中的位置

	// delete pen;
}