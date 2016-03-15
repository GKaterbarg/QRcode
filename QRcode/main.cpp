#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "QRdetector.h"


using namespace cv;
using namespace std;
Point intersection(vector<FinderPattern*> centers);
int main() {
	//VideoCapture capture = VideoCapture(1);
	QRdetector qrDet = QRdetector();

	//if(!capture.isOpened()) {
	//    printf("Unable to open camera. Quitting.");
	//    return 1;
	//}

	Mat image = imread("Images/20.jpg");
	Mat imgBW = Mat(image.rows, image.cols, CV_8UC1);
	//while(true) {
	//    capture >> image;

	cvtColor(image, imgBW, CV_BGR2GRAY);
	threshold(imgBW, imgBW, 128, 255, THRESH_BINARY);

	qrDet.setImg(imgBW);
	vector<FinderPattern*> qrCode = qrDet.find();
	printf("Num centers: (%i)\n", qrCode.size());
	//Point forthPtr = intersectionPoint(qrCode);
	//rectangle(image, Point(qrCode[0]->getX(), qrCode[0]->getY()), Point(qrCode[1]->getX(), qrCode[1]->getY()), Scalar(0, 0, 255), 3);

	for each (FinderPattern *fp in qrCode)
	{
		if (fp != NULL) circle(image, Point(fp->getX(), fp->getY()), 5, (0, 0, 255), -1);
	}

	//circle(image, forthPtr, 5,(0, 0, 255), -1);

	imshow("Original", image);
	imshow("Binary", imgBW);
	waitKey(30);
	//}

	waitKey(0);
	return 0;
}

vector<float> cross(vector<float> a, vector<float> b)
{
	return{	a[1] * b[2] - a[2] * b[1],
			a[2] * b[0] - a[0] * b[2],
			a[0] * b[1] - a[1] * b[0] };
}

Point intersectionPoint(vector<FinderPattern*> centers){

	vector<float> a = { centers[0]->getX(), centers[0]->getY(), 1 };
	vector<float> b = { centers[1]->getX(), centers[1]->getY(), 1 };
	vector<float> c = { centers[2]->getX(), centers[2]->getY(), 1 };

	vector<float> ab = cross(a, b);
	vector<float> bc = cross(b, c);

	vector<float> abParal = { ab[0], ab[1], (-ab[0] * c[0] - ab[1] * c[1]) };
	vector<float> bcParal = { bc[0], bc[1], -bc[0] * a[0] - bc[1] * a[1] };
	vector<float> inters = cross(abParal, bcParal);

	return Point(inters[0] / inters[2], inters[1] / inters[2]);
}
/*
void drawRect(){

}
*/

