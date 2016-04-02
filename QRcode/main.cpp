#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "QRdetector.h"
#include "TestModule.h"
#include<fstream>

using namespace cv;
using namespace std;

int main()
{
	const int num = 20;
	TestModule test = TestModule(num);
	test.startTest();
	//pair<Point, Point> areaCoords = getAreaCoords();
	/*QRdetector qrDet = QRdetector();
	Mat image = imread("Test/5.jpg");
	Mat imgBW = Mat(image.rows, image.cols, CV_8UC1);
	cvtColor(image, imgBW, CV_BGR2GRAY);
	threshold(imgBW, imgBW, 128, 255, THRESH_BINARY); 
	qrDet.setImg(imgBW);
	vector<FinderPattern*> qrCode = qrDet.find(); // qrCode FP centers
	if (qrCode.size() == 3) { 
		Point intersectionPt = qrDet.intersectionPoint(qrCode);
		circle(image, intersectionPt, 5, (0, 0, 255), -1);
	}
	for each (FinderPattern *fp in qrCode) {
		if (fp != NULL) circle(image, Point(fp->getX(), fp->getY()), 5, (0, 0, 255), -1);
	}
	imshow("Original", image);
	imshow("Binary", imgBW);*/
	waitKey(0);
	return 0;
}

