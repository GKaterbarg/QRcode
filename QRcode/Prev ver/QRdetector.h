#ifndef __QR_DETECTOR
#define __QR_DETECTOR

#include <opencv2/opencv.hpp>
#include "FinderPattern.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

class QRdetector {
public:
	void setImg(Mat img);
	vector<FinderPattern*> find();
	bool checkRatio(int currentState[]);
	QRdetector();
	Point intersectionPoint(vector<FinderPattern*> pattern);

private:
	bool checkPossibleCenter(int stateCount[], int i, int j);
	float centerFromEnd(int stateCount[], int end);
	float crossCheckHorizontal(int startCol, int centerRow, int blackSqrCount, int originalStateCountTotal);
	float crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal);
	vector<FinderPattern*> identifyBestPatterns();
	vector<FinderPattern*> orderBestPatterns(vector<FinderPattern*> pattern);
	vector<float> cross(FinderPattern* a, FinderPattern* b);
	vector<float> cross(vector<float> a, vector<float> b);

private:
	int crossCheckStateCount[5];
	bool hasSkipped;
	vector<FinderPattern*> centers;
	Mat img;
};
#endif