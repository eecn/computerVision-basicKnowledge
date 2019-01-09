#include "imgFeat.h"

/*
* @function: specifies the soble method.  指定soble方法。

* @param IMGSRC: the input image    输入图像
* @param IMGDST: output edge image  输出的边界检测图像
* @param THRESH: imgDst ignores all edges that are not stronger than THRESH.  这是阀值 imgDst忽略所有不强于THRESH的边
* If you dont specify THRESH, the THRESH will chooses the value automatically.  不指定阀值的话 自动生成一个
* @param DIRECTION: integer number specifying whether to look for horizontal or vertical edges, or both(the default). 指定方向

* Revision:1.o
* Data:2014/10/6
* Athor:Ronny
*/
double feat::getSobelEdge(const Mat& imgSrc, Mat& imgDst, double thresh, int direction)
{
	Mat gray;     // 只处理灰度图
    if (imgSrc.channels() == 3)
    {
        cvtColor(imgSrc, gray, CV_BGR2GRAY);
    }
    else
    {
        gray = imgSrc.clone();
    }
	// 系数设置  C++ 赋值
	int kx(0);
	int ky(0);
	if (direction == SOBEL_HORZ){    // 水平的话 x设置为0  y设置为1 ；否则  y设置为0
		kx = 0; ky = 1;
	}
	else if (direction == SOBEL_VERT){
		kx = 1; ky = 0;
	}
	else{
		kx = 1; ky = 1;
	}


	// 设置mask
	float mask[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };    //这个是y方向的滤波器
	Mat y_mask = Mat(3, 3, CV_32F, mask) / 8;
	Mat x_mask = y_mask.t(); // 转置

	// 计算x方向和y方向上的滤波
	Mat sobelX, sobelY;
	filter2D(gray, sobelX, CV_32F, x_mask);     //对图像边缘的1像素点设置为0
	filter2D(gray, sobelY, CV_32F, y_mask);
	sobelX = abs(sobelX);
	sobelY = abs(sobelY);
	// 梯度图
	Mat gradient = kx*sobelX.mul(sobelX) + ky*sobelY.mul(sobelY);   // 梯度图 这里没有进行开平方操作

	// 计算阈值
	int scale = 4;
	double cutoff;
	if (thresh = -1)
	{
		cutoff = scale*mean(gradient)[0];
		thresh = sqrt(cutoff);
	}
	else
	{
		cutoff = thresh * thresh;
	}

	imgDst.create(gray.size(), gray.type());
	imgDst.setTo(0);
	for (int i = 1; i<gray.rows - 1; i++)
	{
		float* sbxPtr = sobelX.ptr<float>(i);
		float* sbyPtr = sobelY.ptr<float>(i);
		float* prePtr = gradient.ptr<float>(i - 1);
		float* curPtr = gradient.ptr<float>(i);
		float* lstPtr = gradient.ptr<float>(i + 1);
		uchar* rstPtr = imgDst.ptr<uchar>(i);
		// 阈值化和极大值抑制
		/*
		这里检测到的边缘应该是白色  对于这样的检测边缘一般比较粗糙  所以进行阈值化和极大值抑制
		阈值  当gradient大于cutoff时 如果小的话可以看作其与周围像素的灰度变化不剧烈
		极大值抑制:  如果是竖直边缘，则它的梯度值要比左边和右边的点都大；
					 如果是水平连续，则该点的梯度值要比上边和下边的都大。

		满足这个条件的 则表明该点是比较合理的边缘线上的点
		*/
		for (int j = 1; j<gray.cols - 1; j++)
		{
			if (curPtr[j]>cutoff && (
				(sbxPtr[j]>kx*sbyPtr[j] && curPtr[j]>curPtr[j - 1] && curPtr[j]>curPtr[j + 1]) ||
				(sbyPtr[j]>ky*sbxPtr[j] && curPtr[j]>prePtr[j] && curPtr[j]>lstPtr[j])))
				rstPtr[j] = 255;
		}
	}

	return thresh;
}

