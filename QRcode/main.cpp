#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "TestModule.h"
#include "QrDetectorMod.h"

using namespace cv;
using namespace std;

Mat img;

//float getAreaRect(vector<vector<Point>> qrCode);

int main()
{
	TestModule test = TestModule(20);
	test.startTest();

	/*img = imread("Test/15.jpg");
	QrDetectorMod qrDet = QrDetectorMod(img);
	vector<Point> fps = qrDet.find();
	for each (Point c in fps){

			circle(img, c, 5, Scalar(0, 0, 255), -1);
			
	}
	imshow("Original", img);*/
	waitKey(0);
	return 0;
}



