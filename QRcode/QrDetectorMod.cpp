#include "QrDetectorMod.h"


QrDetectorMod::QrDetectorMod(Mat image){
	this -> image = image;
}


vector<vector<Point>> QrDetectorMod::find() {

	Mat gray = Mat(image.rows, image.cols, CV_8UC1);
	Mat edges(image.size(), CV_MAKETYPE(image.depth(), 1));
	cvtColor(image, gray, CV_BGR2GRAY);
	Canny(gray, edges, 100, 200, 3);

	vector<vector<Point> > contours;
	vector<Point> approx;
	findContours(edges, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	drawContours(edges, contours, -1, Scalar(255, 0, 0), CV_FILLED); //for debug
	//imshow("Edges", edges); //for debug

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.04, true);
		if (approx.size() == 4 && fabs(contourArea(Mat(approx))) > 10 && isContourConvex(Mat(approx))){
			drawContours(image, contours, i, Scalar(255, 0, 0), CV_FILLED); //for debug
			if (isQuad(&approx) && !inOtherContour(approx)){
				quadList.push_back(vector<Point>(approx));
			}
		}
	}

	vector<vector<Point>> fps;
	Mat image2;
	for each(vector<Point> quad in quadList){

		Point min = minCoord(quad);
		Point max = maxCoord(quad);

		int x = min.x - 0.2*(max.x - min.x),
			y = min.y - 0.2*(max.y - min.y);
		if (x < 0) x = 0; if (y < 0) y = 0;

		int	w = 1.8*(max.x - min.x),
			h = 1.8*(max.y - min.y);
		if (x + w > gray.cols) w = gray.cols - x - 1;
		if (h + y > gray.rows) h = gray.rows - y - 1;
		image2 = gray(Rect(x, y, w, h));
		threshold(image2, image2, 128, 255, THRESH_OTSU);
		//imshow("Parts", image2);//for debug
		if (horizontalCheck(image2)) fps.push_back(quad);
		//waitKey(1200);//for debug
	}

	return fps;
}


Point QrDetectorMod::minCoord(vector<Point> v) {
	float minY = v[0].y;
	float minX = v[0].x;
	for (int i = 1; i < 4; i++) {
		if (v[i].y < minY) minY = v[i].y;
		if (v[i].x < minX) minX = v[i].x;

	}

	return Point(minX, minY);
}


Point QrDetectorMod::maxCoord(vector<Point> v) {
	float maxY = v[0].y;
	float maxX = v[0].x;
	for (int i = 1; i < 4; i++) {
		if (v[i].y > maxY) maxY = v[i].y;
		if (v[i].x > maxX) maxX = v[i].x;

	}

	return Point(maxX, maxY);
}


float QrDetectorMod::dist(Point v1, Point v2) {
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;

	return sqrt(dx*dx + dy*dy);
}


bool QrDetectorMod::inOtherContour(vector<Point> test) {

	RotatedRect testRect = minAreaRect(test);

	for (int i = 0; i < quadList.size(); i++) {

		RotatedRect rect = minAreaRect(quadList[i]);

		if (dist(rect.center, testRect.center) < 10) {
			if (testRect.size.area() > rect.size.area()) {
				quadList[i] = test;
				return true;
			}
			return true;
		}
	}

	return false;
}


bool QrDetectorMod::isQuad(vector<Point>* quad) {

	vector<Point> pts = *quad;
	for (int i = 0; i < 2; i++) {
		for (int j = i + 1; j < 4; j++) {
			if (pts[i].y > pts[j].y){
				swap(pts[i], pts[j]);
			}
		}
	}
	if (pts[0].x > pts[1].x) swap(pts[0], pts[1]);
	if (pts[2].x > pts[3].x) swap(pts[2], pts[3]);
	*quad = pts;

	float a = dist(pts[0], pts[2]);
	float b = dist(pts[0], pts[1]);
	float c = dist(pts[1], pts[3]);
	float d = dist(pts[2], pts[3]);
	if (a < c) swap(a, c);
	if (b < d) swap(b, d);

	//float ca = c / a;//for debug
	//float db = d / b;//for debug
	//float ba = b / a;//for debug
	//float da = d / a;//for debug

	if (c / a < 0.9 || d / b < 0.9) return false;
	if (b / a < 0.8 || a / b < 0.8) return false;
	if (d / a < 0.8 || a / d < 0.8) return false;

	return true;
}


bool QrDetectorMod::checkRatio(int stateCount[]) {
	int totalFinderSize = 0;
	for (int i = 0; i<5; i++) {
		int count = stateCount[i];
		totalFinderSize += count;
		if (count == 0)
			return false;
	}

	if (totalFinderSize<7) {
		return false;
	}

	int moduleSize = ceil(totalFinderSize / 7.0);
	float maxVariance = moduleSize * 0.5;

	bool retVal = ((abs(moduleSize - stateCount[0])) <= maxVariance &&
		(abs(moduleSize - stateCount[1])) <= maxVariance &&
		(abs(3 * moduleSize - stateCount[2])) < 3 * maxVariance &&
		(abs(moduleSize - stateCount[3])) <= maxVariance &&
		(abs(moduleSize - stateCount[4])) <= maxVariance);

	return retVal;
}


float QrDetectorMod::centerFromEnd(int stateCount[], int end) {
	return (float)(end - stateCount[4] - stateCount[3]) - stateCount[2] / 2.0f;
}


bool QrDetectorMod::horizontalCheck(Mat img) {

	int skipRows = 3;
	int stateCount[5] = { 0 };
	int currentState = 0;

	for (int row = skipRows - 1; row<img.rows; row += skipRows) {

		stateCount[0] = 0;
		stateCount[1] = 0;
		stateCount[2] = 0;
		stateCount[3] = 0;
		stateCount[4] = 0;
		currentState = 0;

		uchar *ptr = img.ptr<uchar>(row);
		for (int col = 0; col<img.cols; col++) {
			// black pixel
			if (ptr[col]<128) {
				if ((currentState & 0x1) == 1) {
					currentState++;
				}

				stateCount[currentState]++;

			}
			else {
				// white pixel
				if ((currentState & 0x1) == 1) {
					stateCount[currentState]++;
				}
				else {

					if (currentState == 4) {

						if (checkRatio(stateCount)) {
							int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
							float centerCol = centerFromEnd(stateCount, col);
							bool confirmed = crossCheckVertical(row, (int)centerCol, stateCount[2], stateCountTotal, img);
							if (confirmed) {
								return true;
							}
							else {
								stateCount[0] = stateCount[2];
								stateCount[1] = stateCount[3];
								stateCount[2] = stateCount[4];
								stateCount[3] = 1;
								stateCount[4] = 0;
								currentState = 3;
								continue;
							}

							currentState = 0;
							stateCount[0] = 0;
							stateCount[1] = 0;
							stateCount[2] = 0;
							stateCount[3] = 0;
							stateCount[4] = 0;

						}

						else {

							currentState = 3;
							stateCount[0] = stateCount[2];
							stateCount[1] = stateCount[3];
							stateCount[2] = stateCount[4];
							stateCount[3] = 1;
							stateCount[4] = 0;

						}

					}

					else {

						stateCount[++currentState]++;

					}
				}
			}
		}
		// end looping current row
		//if (checkRatio(stateCount)) {
		//	bool confirmed = handlePossibleCenter(stateCount, row, img.cols);
		//	if (confirmed) {
		//		skipRows = stateCount[0];
		//
		//	}
		//}
	}

	return false;
}


bool QrDetectorMod::crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal, Mat img) {
	int stateCount[5] = { 0 };

	int row = startRow;
	int maxRow = img.rows;
	while (row >= 0 && img.at<uchar>(row, centerCol) < 128) {
		stateCount[2]++;
		row--;
	}
	if (row<0) {
		return NAN;
	}

	while (row >= 0 && img.at<uchar>(row, centerCol) > 128 && stateCount[1] <= blackSqrCount) {
		stateCount[1]++;
		row--;
	}
	if (row<0 || stateCount[1] > blackSqrCount) {
		return NAN;
	}

	while (row >= 0 && img.at<uchar>(row, centerCol) < 128 && stateCount[0] <= blackSqrCount) {
		stateCount[0]++;
		row--;
	}
	if (stateCount[0]>blackSqrCount) {
		return NAN;
	}

	// Now start moving down from the center
	row = startRow + 1;
	while (row<maxRow && img.at<uchar>(row, centerCol) < 128) {
		stateCount[2]++;
		row++;
	}
	if (row == maxRow) {
		return NAN;
	}

	while (row<maxRow && img.at<uchar>(row, centerCol) > 128 && stateCount[3]<blackSqrCount) {
		stateCount[3]++;
		row++;
	}
	if (row == maxRow || stateCount[3]>blackSqrCount) {
		return NAN;
	}

	while (row<maxRow && img.at<uchar>(row, centerCol) < 128 && stateCount[4] < blackSqrCount) {
		stateCount[4]++;
		row++;
	}
	if (stateCount[4] >= blackSqrCount) {
		return NAN;
	}

	return checkRatio(stateCount);
}


