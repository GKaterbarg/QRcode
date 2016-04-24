#ifndef __QR_DETECTOR_MOD
#define __QR_DETECTOR_MOD

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "FP.hpp"

using namespace cv;
using namespace std;

class QrDetectorMod {
public:
	QrDetectorMod (Mat image);
	vector<FP> find();

private:
	vector<FP> QrDetectorMod::orderBestPatterns(vector<FP> pattern);
	vector<int> cross(Point a, Point b);
	vector<int> cross(vector<int> a, vector<int> b);
	vector<int> cross(FP a, FP b);
	double dist(Point v1, Point v2);
	double dist(FP v1, FP v2);
	int area(vector<Point> quad);
	Point getCenter(vector<Point> quad);
	bool isQuad(vector<Point>* pts);
	bool inOtherContour(vector<Point>* test);
	Point minCoord(vector<Point> v);
	Point maxCoord(vector<Point> v);
	bool horizontalCheck(Mat img);
	bool checkRatio(int stateCount[]);
	float centerFromEnd(int stateCount[], int end);
	bool crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal, Mat img);
	bool firstHorizontalCheck(Mat img, int row);
	Point QrDetectorMod::intersectionPoint(vector<FP> fps);

private:
	vector<vector<Point>> quadList;
	double module;
	Mat image;
};
#endif