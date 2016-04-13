#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "TestModule.h"
#include "QrDetectorMod.h"

using namespace cv;
using namespace std;


int main()
{
	const int n = 20;
	TestModule test = TestModule(n);
	test.startTest();
	/*Mat img = imread("Test/7.jpg");
	QrDetectorMod qrDet = QrDetectorMod(img);
	vector<vector<Point>> fps = qrDet.find();
	for each (vector<Point> fp in fps){
		for each(Point pt in fp){
			circle(img, pt, 5, Scalar(0, 0, 255), -1);
		}
			
	}*/
	//imshow("Original", img);
	waitKey(0);
	return 0;
}


