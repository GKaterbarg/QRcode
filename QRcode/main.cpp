#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "TestModule.h"
#include "QrDetectorMod.h"

using namespace cv;
using namespace std;

void getAreaRect(vector<FP> qrCode, Mat image);
int videoInput();
void singleInput();

int main()
{
	//if (videoInput()) return 1; // Video or stream input

	//TestModule test = TestModule(20); test.startTest(); // Test some number images

	singleInput(); // Single image input
	
	waitKey(0);
	return 0;
}

void singleInput(){

	QrDetectorMod qrDet = QrDetectorMod();
	Mat img = imread("Test/0.jpg");
	qrDet.setImage(img);
	vector<FP> fps = qrDet.find();

	for each (FP c in fps) circle(img, Point(c.x, c.y), 5, Scalar(0, 0, 255), -1); 

	if (fps.size() > 3) getAreaRect(fps, img);
	imshow("Original", img);
}

int videoInput(){

	Mat img;
	QrDetectorMod qrDet = QrDetectorMod();
	VideoCapture capture = VideoCapture("Images/vlc-record-1.avi");
	if (!capture.isOpened()) {
		printf("Unable to open camera. Quitting.");
		return 1;
	}
	while (true) {
		capture >> img;
		qrDet.setImage(img);
		vector<FP> fps = qrDet.find();

		for each (FP c in fps) circle(img, Point(c.x, c.y), 5, Scalar(0, 0, 255), -1);
		
		if (fps.size() > 3) getAreaRect(fps, img);
		imshow("Original", img);
		waitKey(30);
	}
	return 0;

}

void getAreaRect(vector<FP> qrCode, Mat image) {
	int maxY = 0, minY = image.rows, maxX = 0, minX = image.cols;
	int x, y;
	int interval = 4 * qrCode[0].module;

	for (int i = 0; i < 4; i++) {
		if ((y = qrCode[i].y) < minY) minY = y;
		if ((y = qrCode[i].y) > maxY) maxY = y;
		if ((x = qrCode[i].x) < minX) minX = x;
		if ((x = qrCode[i].x) > maxX) maxX = x;
	}

	rectangle(image, Point(minX - interval, maxY + interval), Point(maxX + interval, minY - interval), Scalar(0, 0, 255), 3);
}



