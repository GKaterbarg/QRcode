#ifndef __QR_DETECTOR_MOD
#define __QR_DETECTOR_MOD

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "FP.hpp"

using namespace cv;
using namespace std;

class QrDetectorMod {
public:
	QrDetectorMod ();
	void setImage(Mat image);
	vector<FP> find();

private:
	vector<FP> orderBestPatterns(vector<FP> pattern);
	vector<int> cross(Point a, Point b);
	vector<int> cross(FP a, FP b);
	vector<int> cross(vector<int> a, vector<int> b);
	double dist(Point v1, Point v2);
	double dist(FP v1, FP v2);
	Point minCoord(vector<Point> v);
	Point maxCoord(vector<Point> v);
	int area(vector<Point> quad);
	Point getCenter(vector<Point> quad);
	bool isQuad(vector<Point>* pts);
	bool inOtherContour(vector<Point>* test);
	bool firstHorizontalCheck(Mat img, int row);
	bool horizontalCheck(Mat img);
	bool checkRatio(int stateCount[]);
	float centerFromEnd(int stateCount[], int end);
	bool crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal, Mat img);
	Point intersectionPoint(vector<FP> fps);
	float getAreaRect(vector<FP> qrCode);
	vector<Point> simplifyWithRDP(vector<Point>& points, int startIndex, int lastIndex, double epsilon);
	double pointLineDistance(Point point, Point start, Point end);
	int findLastPoint(vector<Point> pts);
	double contourLength(vector<Point> contour);
	vector<Point> approximate(vector<Point> contour);

private:
	vector<vector<Point>> quadList;
	double module;
	Mat image;
};
#endif