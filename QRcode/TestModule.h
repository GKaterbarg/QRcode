#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "QrDetectorMod.h"
#include "QRdetector.h"
#include<fstream>


class TestModule {
public:
	TestModule(int numImages);
	void startTest();

private:
	vector<vector<Point>> readRealCoords();
	void drawPoints(vector<Point> realCoords, vector<FinderPattern*> qrCode, Point intersPt);
	float getArea(vector<Point> coords);
	float dist(Point v1, Point v2);
	float getAreaRect(vector<FinderPattern*> qrCode, Point intersPt);
	float getAreaRect(vector<vector<Point>> qrCode);

private:
	int numImages;
	Mat img;
};