#include "imgFeat.h"


//对亮度和对比度的变化不敏感  具有旋转不变性  不具有尺度不变性
void feat::detectHarrisCorners(const Mat& imgSrc, Mat& imgDst, double alpha)
{
	Mat gray;
	if (imgSrc.channels() == 3)
	{
		cvtColor(imgSrc, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = imgSrc.clone();
	}
	gray.convertTo(gray, CV_64F);

	Mat xKernel = (Mat_<double>(1,3) << -1, 0, 1);    //求近似梯度的 kernel
	Mat yKernel = xKernel.t();

	Mat Ix,Iy;
	filter2D(gray, Ix, CV_64F, xKernel);
	filter2D(gray, Iy, CV_64F, yKernel);

	Mat Ix2,Iy2,Ixy;
	Ix2 = Ix.mul(Ix);
	Iy2 = Iy.mul(Iy);
	Ixy = Ix.mul(Iy);

	Mat gaussKernel = getGaussianKernel(7, 1);
	filter2D(Ix2, Ix2, CV_64F, gaussKernel);  //A
	filter2D(Iy2, Iy2, CV_64F, gaussKernel);  //C
	filter2D(Ixy, Ixy, CV_64F, gaussKernel);  //B
	

	Mat cornerStrength(gray.size(), gray.type());
	for (int i = 0; i < gray.rows; i++)
	{
		for (int j = 0; j < gray.cols; j++)
		{
			double det_m = Ix2.at<double>(i,j) * Iy2.at<double>(i,j) - Ixy.at<double>(i,j) * Ixy.at<double>(i,j); //AC-B^2
			double trace_m = Ix2.at<double>(i,j) + Iy2.at<double>(i,j);											  //A + C
			cornerStrength.at<double>(i,j) = det_m - alpha * trace_m *trace_m;  //角点响应值 R  对应的也是一个矩阵

		}
	}
	// threshold  阈值化
	double maxStrength;
	minMaxLoc(cornerStrength, NULL, &maxStrength, NULL, NULL);  // 找到图像中的最大、最小值
	Mat dilated;
	Mat localMax;
	dilate(cornerStrength, dilated, Mat());    // 膨胀图像，最找出图像中全部的局部最大值点
	compare(cornerStrength, dilated, localMax, CMP_EQ);// compare是一个逻辑比较函数，返回两幅图像中对应点相同的二值图像
	

	Mat cornerMap;
	double qualityLevel = 0.01;
	double thresh = qualityLevel * maxStrength; //阈值计算
	//threshold(cornerStrength, cornerMap, th, 255, THRESH_BINARY);
	//cornerMap.convertTo(cornerMap, CV_8U);

	cornerMap = cornerStrength > thresh;
	// 逐点的位运算
	bitwise_and(cornerMap, localMax, cornerMap);
	
	imgDst = cornerMap.clone();
	
}

void feat::drawCornerOnImage(Mat& image, const Mat&binary)
{
    Mat_<uchar>::const_iterator it = binary.begin<uchar>();
    Mat_<uchar>::const_iterator itd = binary.end<uchar>();
    for (int i = 0; it != itd; it++, i++)
    {
        if (*it)
			circle(image, Point(i%image.cols, i / image.cols), 3, Scalar(0, 255, 0), 1);  //绿色圆 标识角点
    }
}

