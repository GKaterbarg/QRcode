#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "QRdetector.h"


using namespace cv;
using namespace std;

void drawRect(vector<FinderPattern*> qrCode, Point intersPt, Mat img);

int main() {
	//VideoCapture capture = VideoCapture(1); // for video input
	QRdetector qrDet = QRdetector();

	//if(!capture.isOpened()) {
	//    printf("Unable to open camera. Quitting.");
	//    return 1;
	//}

	Mat image = imread("Images/11.jpg");
	Mat imgBW = Mat(image.rows, image.cols, CV_8UC1);
	//while(true) {
	//    capture >> image;

	cvtColor(image, imgBW, CV_BGR2GRAY);
	threshold(imgBW, imgBW, 128, 255, THRESH_BINARY); 

	qrDet.setImg(imgBW);
	vector<FinderPattern*> qrCode = qrDet.find(); // qrCode FP centers
	if (qrCode.size() == 3){ 
		Point intersectionPt = qrDet.intersectionPoint(qrCode);
		drawRect(qrCode, intersectionPt, image);
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

void drawRect(vector<FinderPattern*> qrCode, Point intersPt, Mat img){

	float maxY = 0.0;
	float minY =  img.rows;
	float y = 0.0;
	int interval = 4 * ceil(qrCode[1]->getEstimatedModuleSize());
	for (int i = 0; i < 3; i++){
		if ((y = qrCode[i]->getY()) < minY) minY = y;
		if ((y = qrCode[i]->getY()) > maxY) maxY = y;
	}
	if (maxY < intersPt.y) maxY = intersPt.y;
	if (minY > intersPt.y) minY = intersPt.y;

	rectangle(img, Point(qrCode[0]->getX() - interval, maxY + interval), Point(qrCode[2]->getX() + interval, minY - interval), Scalar(0, 0, 255), 3);

}


