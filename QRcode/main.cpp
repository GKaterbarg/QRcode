#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "QRdetector.h"


using namespace cv;
using namespace std;

float areaRect(vector<FinderPattern*> qrCode, Point intersPt, Mat img);

int main() {
	//VideoCapture capture = VideoCapture(1); // for video input
	QRdetector qrDet = QRdetector();

	//if(!capture.isOpened()) {
	//    printf("Unable to open camera. Quitting.");
	//    return 1;
	//}

	Mat image = imread("Images/0,4/3.jpg");
	Mat imgBW = Mat(image.rows, image.cols, CV_8UC1);
	//while(true) {
	//    capture >> image;

	cvtColor(image, imgBW, CV_BGR2GRAY);
	threshold(imgBW, imgBW, 128, 255, THRESH_OTSU); 

	qrDet.setImg(imgBW);
	vector<FinderPattern*> qrCode = qrDet.find(); // qrCode FP centers
	if (qrCode.size() == 3){ 
		Point intersectionPt = qrDet.intersectionPoint(qrCode);
		float qrarea = areaRect(qrCode, intersectionPt, image);
		float area = image.cols*image.rows;
		printf("Area = (%f)\n", qrarea / area);
		circle(image, intersectionPt, 5, (0, 0, 255), -1);
	}

	for each (FinderPattern *fp in qrCode)
	{
		if (fp != NULL) circle(image, Point(fp->getX(), fp->getY()), 5, (0, 0, 255), -1);
	}

	imshow("Original", image);
	imshow("Binary", imgBW);
	waitKey(30);
	//}

	waitKey(0);
	return 0;
}

float areaRect(vector<FinderPattern*> qrCode, Point intersPt, Mat img){
	float maxY = 0.0;
	float minY =  img.rows;
	float maxX = 0.0;
	float minX = img.cols;
	float x,y;
	int interval = 4 * ceil(qrCode[1]->getEstimatedModuleSize());
	for (int i = 0; i < 3; i++){
		if ((y = qrCode[i]->getY()) < minY) minY = y;
		if ((y = qrCode[i]->getY()) > maxY) maxY = y;
		if ((x = qrCode[i]->getX()) < minX) minX = x;
		if ((x = qrCode[i]->getX()) > maxX) maxX = x;
	}
	if (maxY < intersPt.y) maxY = intersPt.y;
	if (minY > intersPt.y) minY = intersPt.y;
	if (maxX < intersPt.x) maxX = intersPt.x;
	if (minX > intersPt.x) minX = intersPt.x;

	rectangle(img, Point(minX - interval, maxY + interval), Point(maxX + interval, minY - interval), Scalar(0, 0, 255), 3);

	return (maxX - minX + 2*interval) * (maxY - minY + 2*interval);
}



