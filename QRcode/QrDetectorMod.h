#ifndef __QR_DETECTOR_MOD
#define __QR_DETECTOR_MOD

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class QrDetectorMod {
public:
	QrDetectorMod (Mat image);
	vector<vector<Point>> find();

private:
	bool isQuad(vector<Point>* pts);
	bool inOtherContour(vector<Point> test);
	Point minCoord(vector<Point> v);
	Point maxCoord(vector<Point> v);
	bool horizontalCheck(Mat img);
	bool checkRatio(int stateCount[]);
	float centerFromEnd(int stateCount[], int end);
	float dist(Point v1, Point v2);
	bool crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal, Mat img);

private:
	vector<vector<Point>> quadList;
	Mat image;
};
#endif