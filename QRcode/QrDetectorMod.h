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
	uchar** QrDetectorMod::matToArr(Mat img);
	// Contour analysys
	void findContours_(uchar** img, vector<vector<Point>>* contours);
	void findContour(unsigned char** img, vector<Point>* contour, int y, int x, int k);
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
	// Binary image analysys
	bool fullHorizontalCheck(Mat img);
	bool singleHorizontalCheck(Mat img, int row);
	bool checkRatio(int stateCount[]);
	float centerFromEnd(int stateCount[], int end);
	bool crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal, Mat img);
	float getAreaRect(vector<FP> qrCode);
	// Approximation
	vector<Point> simplifyWithRDP(vector<Point>& points, int startIndex, int lastIndex, double epsilon);
	double pointLineDistance(Point point, Point start, Point end);
	pair<int,int> findFurthestPts(vector<Point> pts);
	double contourLength(vector<Point> contour);
	int removeExtraPoints(vector<Point>* dst, double eps);
	vector<Point> approximate(vector<Point> contour);
	// Final work with FPs
	Point intersectionPoint(vector<FP> fps);
	vector<FP> orderBestPatterns(vector<FP> pattern);

private:
	vector<vector<Point>> quadList;
	double module;
	Mat image;
	int IMAGE_HEIGTH;
	int IMAGE_WIDTH;
};
#endif