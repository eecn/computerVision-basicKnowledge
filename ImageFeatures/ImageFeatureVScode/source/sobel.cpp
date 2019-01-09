#include "imgFeat.h"

/*
* @function: specifies the soble method.  ָ��soble������

* @param IMGSRC: the input image    ����ͼ��
* @param IMGDST: output edge image  ����ı߽���ͼ��
* @param THRESH: imgDst ignores all edges that are not stronger than THRESH.  ���Ƿ�ֵ imgDst�������в�ǿ��THRESH�ı�
* If you dont specify THRESH, the THRESH will chooses the value automatically.  ��ָ����ֵ�Ļ� �Զ�����һ��
* @param DIRECTION: integer number specifying whether to look for horizontal or vertical edges, or both(the default). ָ������

* Revision:1.o
* Data:2014/10/6
* Athor:Ronny
*/
double feat::getSobelEdge(const Mat& imgSrc, Mat& imgDst, double thresh, int direction)
{
	Mat gray;     // ֻ����Ҷ�ͼ
    if (imgSrc.channels() == 3)
    {
        cvtColor(imgSrc, gray, CV_BGR2GRAY);
    }
    else
    {
        gray = imgSrc.clone();
    }
	// ϵ������  C++ ��ֵ
	int kx(0);
	int ky(0);
	if (direction == SOBEL_HORZ){    // ˮƽ�Ļ� x����Ϊ0  y����Ϊ1 ������  y����Ϊ0
		kx = 0; ky = 1;
	}
	else if (direction == SOBEL_VERT){
		kx = 1; ky = 0;
	}
	else{
		kx = 1; ky = 1;
	}


	// ����mask
	float mask[3][3] = { { 1, 2, 1 }, { 0, 0, 0 }, { -1, -2, -1 } };    //�����y������˲���
	Mat y_mask = Mat(3, 3, CV_32F, mask) / 8;
	Mat x_mask = y_mask.t(); // ת��

	// ����x�����y�����ϵ��˲�
	Mat sobelX, sobelY;
	filter2D(gray, sobelX, CV_32F, x_mask);     //��ͼ���Ե��1���ص�����Ϊ0
	filter2D(gray, sobelY, CV_32F, y_mask);
	sobelX = abs(sobelX);
	sobelY = abs(sobelY);
	// �ݶ�ͼ
	Mat gradient = kx*sobelX.mul(sobelX) + ky*sobelY.mul(sobelY);   // �ݶ�ͼ ����û�н��п�ƽ������

	// ������ֵ
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
		// ��ֵ���ͼ���ֵ����
		/*
		�����⵽�ı�ԵӦ���ǰ�ɫ  ���������ļ���Եһ��Ƚϴֲ�  ���Խ�����ֵ���ͼ���ֵ����
		��ֵ  ��gradient����cutoffʱ ���С�Ļ����Կ���������Χ���صĻҶȱ仯������
		����ֵ����:  �������ֱ��Ե���������ݶ�ֵҪ����ߺ��ұߵĵ㶼��
					 �����ˮƽ��������õ���ݶ�ֵҪ���ϱߺ��±ߵĶ���

		������������� ������õ��ǱȽϺ���ı�Ե���ϵĵ�
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

