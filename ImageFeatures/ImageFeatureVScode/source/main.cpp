#include "imgFeat.h"

int main(int argc, char** argv)
{
	Mat image = imread(argv[1]);
	Mat cornerMap;
	Mat edgeMap;
	/*
	feat::detectHarrisLaplace(image ,cornerMap);
	feat::drawCornerOnImage(image, cornerMap);
	namedWindow("corners");
	imshow("corners", image);
	*/

	/* SobelËã×Ó
	feat::getSobelEdge(image, edgeMap);
	namedWindow("edge");
	imshow("edge", edgerMap);
	*/

	/*
	feat::detectHarrisCorners(image, cornerMap,0.1);
	feat::drawCornerOnImage(image, cornerMap);
	namedWindow("corners");
	imshow("corners", image);
	*/

	feat::getCannyEdge(image, edgeMap);
	namedWindow("edge");
	imshow("edge", edgeMap);
	
	waitKey();
	return 0;
}
