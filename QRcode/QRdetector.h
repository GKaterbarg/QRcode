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

private:
	bool checkPossibleCenter(int stateCount[], int i, int j);
	float centerFromEnd(int stateCount[], int end);
	float crossCheckHorizontal(int startCol, int centerRow, int blackSqrCount, int originalStateCountTotal);
	float crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal);
	vector<FinderPattern*> identifyBestPatterns();
	vector<FinderPattern*> orderBestPatterns(vector<FinderPattern*> pattern);
	int getRowSkip();

private:
	int crossCheckStateCount[5];
	bool hasSkipped;
	std::vector<FinderPattern*> centers;
	Mat img;
};