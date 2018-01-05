
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>
#include "sketchize.h"
using namespace std;
using namespace cv;

int IterationThreshold(CvMat* gray)
{
	int width = gray->width;
	int height = gray->height;

	//直方图统计  
	int histData[256] = { 0 };
	for (int j = 0; j < height; j++)
	{
		uchar*data = (uchar*)(gray->data.ptr + j * gray->step);
		for (int i = 0; i < width; i++)
		{
			histData[data[i]]++;
		}
	}

	//求图像的平均灰度值作为初始阈值  
	int T0 = 0;
	for (int i = 0; i < 256; i++)
	{
		T0 += i * histData[i];
	}
	T0 /= width * height;

	//迭代  
	int T1 = 0, T2 = 0;
	int num1 = 0, num2 = 0;
	int T = 0;
	while (1)
	{
		for (int i = 0; i < T0 + 1; i++)
		{
			T1 += i * histData[i];
			num1 += histData[i];
		}

		if (num1 == 0)
			continue;

		for (int i = T0 + 1; i < 256; i++)
		{
			T2 += i * histData[i];
			num2 += histData[i];
		}

		if (num2 == 0)
			continue;

		T = (T1 / num1 + T2 / num2) / 2;

		if (T == T0)
			break;
		else
			T0 = T;
	}

	return T;
}


int myOtsu(const IplImage *frame) //大津法求阈值   
{
#define GrayScale 256   //frame灰度级   
	int width = frame->width;
	int height = frame->height;
	int pixelCount[GrayScale] = { 0 };
	float pixelPro[GrayScale] = { 0 };
	int i, j, pixelSum = width * height, threshold = 0;
	uchar* data = (uchar*)frame->imageData;

	//统计每个灰度级中像素的个数 
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			pixelCount[(int)data[i * width + j]]++;
		}
	}

	//计算每个灰度级的像素数目占整幅图像的比例   
	for (i = 0; i < GrayScale; i++)
	{
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
	}

	//遍历灰度级[0,255],寻找合适的threshold   
	float w0, w1, u0tmp, u1tmp, u0, u1, deltaTmp, deltaMax = 0;
	for (i = 0; i < GrayScale; i++)
	{
		w0 = w1 = u0tmp = u1tmp = u0 = u1 = deltaTmp = 0;
		for (j = 0; j < GrayScale; j++)
		{
			if (j <= i)   //背景部分   
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			}
			else   //前景部分   
			{
				w1 += pixelPro[j];
				u1tmp += j * pixelPro[j];
			}
		}
		u0 = u0tmp / w0;
		u1 = u1tmp / w1;
		deltaTmp = (float)(w0 *w1* pow((u0 - u1), 2));
		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = i;
		}
	}
	return threshold;
}
int main()
{
	const char* imagename = "emosue.jpg";

	//从文件中读入图像
	Mat img = imread("F:\\8.jpg");
	sketchize ize;
	
	//如果读入图像失败
	if (img.empty())
	{
		fprintf(stderr, "Can not load image %s\n", imagename);
		return -1;
	}
	cvtColor(img, img, CV_BGR2GRAY);
	//Size size;
	//size.width = img.cols / 7;
	//size.height = img.rows / 7;
	//Mat smallImg = Mat(size, CV_8UC1);
 //   //scale image 1/4 smaller
	//resize(img, smallImg, size, 0, 0, INTER_LINEAR);

	
	//imshow("smallimage", img);
	IplImage target(img);

	
	int yuzhi = myOtsu(&target);
	imshow("image", img);
	Mat result;
	threshold(img, result, yuzhi, 255, CV_THRESH_BINARY_INV);
	cout << yuzhi << endl;
	//显示图像
	imshow("ost", result);

	/*CvMat temp = img;
	yuzhi = IterationThreshold((&temp));
	cout << yuzhi << endl;
	threshold(img, result, yuzhi, 255, CV_THRESH_BINARY_INV);
	imshow("iteration", result);
	cout << result.rows << " " << result.cols << endl;*/
	ofstream file("F:\\a.txt");
	for (int i = 0; i < result.rows; i++)
	{
		uchar* data = result.ptr<uchar>(i);
		for (int j = 0; j < result.cols; j++)
		{
			if (data[j] == 0)
				file << " ";
			else
				file << "*";
		}
		file << endl;

	}
	//此函数等待按键，按键盘任意键就返回
	waitKey();

	return 0;
}
