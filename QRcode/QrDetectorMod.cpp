#include "QrDetectorMod.h"

QrDetectorMod::QrDetectorMod(Mat image){
	this -> image = image;
}


vector<FP> QrDetectorMod::find() {

	Mat gray = Mat(image.rows, image.cols, CV_8UC1);
	Mat edges(image.size(), CV_MAKETYPE(image.depth(), 1));
	cvtColor(image, gray, CV_BGR2GRAY);
	Canny(gray, edges, 100, 200, 3);

	vector<vector<Point>> contours;
	vector<Point> approx;
	findContours(edges, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.04, true); // TODO: rewrite approxPolyDP (Ramer–Douglas–Peucker algorithm)
		if (approx.size() == 4 && isQuad(&approx) && abs(area(approx)) > 10){
			drawContours(image, contours, i, Scalar(255, 0, 0), CV_FILLED); //for debug
			if (!inOtherContour(&approx)){
				quadList.push_back(vector<Point>(approx));
			}
		}
	}

	vector<FP> fps;
	for each(vector<Point> quad in quadList){

		Point min = minCoord(quad);
		Point max = maxCoord(quad);

		int x = min.x - 0.4*(max.x - min.x),
			y = min.y - 0.4*(max.y - min.y);
		if (x < 0) x = 0; if (y < 0) y = 0;

		int	w = 2*(max.x - min.x),
			h = 2*(max.y - min.y);
		if (x + w > gray.cols) w = gray.cols - x - 1;
		if (h + y > gray.rows) h = gray.rows - y - 1;
		Mat partImg = gray(Rect(x, y, w, h));
		threshold(partImg, partImg, 128, 255, THRESH_OTSU);
		//imshow("Parts", partImg);//for debug
		if (firstHorizontalCheck(partImg, quad[4].y - y)) {
			fps.push_back(FP(quad[4].x, quad[4].y, module));
		}
		else {
				if (horizontalCheck(partImg)) {
					fps.push_back(FP(quad[4].x, quad[4].y, module));
				}
		}
		//waitKey(1200);//for debug
	}

	Point intersPt;
	fps = orderBestPatterns(fps);
	if (fps.size() == 3) intersPt = intersectionPoint(fps);
	fps.push_back(FP(intersPt.x, intersPt.y, -1));

	return fps;
}

Point QrDetectorMod::intersectionPoint(vector<FP> fps) {

	vector<int> ab = cross(fps[0], fps[1]);
	vector<int> bc = cross(fps[1], fps[2]);

	vector<int> abParal = { ab[0], ab[1], -ab[0] * fps[2].x - ab[1] * fps[2].y };
	vector<int> bcParal = { bc[0], bc[1], -bc[0] * fps[0].x - bc[1] * fps[0].y };
	vector<int> inters = cross(abParal, bcParal);

	return Point(inters[0] / inters[2], inters[1] / inters[2]);
}


// 0 and 2 elements of returned vec locate on hypotenuse, 0 - left FP, 2 - right FP 
vector<FP> QrDetectorMod::orderBestPatterns(vector<FP> pattern) {
	double distance01 = dist(pattern[0], pattern[1]);
	double  distance12 = dist(pattern[1], pattern[2]);
	double  distance02 = dist(pattern[0], pattern[2]);

	vector<FP> returnPatterns(3);

	if (distance12 >= distance01 && distance12 >= distance02) {
		if (pattern[1].x < pattern[2].x){
			returnPatterns[0] = pattern[1];
			returnPatterns[1] = pattern[0];
			returnPatterns[2] = pattern[2];
		}
		else{
			returnPatterns[0] = pattern[2];
			returnPatterns[1] = pattern[0];
			returnPatterns[2] = pattern[1];
		}
	}
	else if (distance02 >= distance01 && distance02 >= distance12) {
		if (pattern[0].x < pattern[2].x){
			returnPatterns[0] = pattern[0];
			returnPatterns[1] = pattern[1];
			returnPatterns[2] = pattern[2];
		}
		else{
			returnPatterns[0] = pattern[2];
			returnPatterns[1] = pattern[1];
			returnPatterns[2] = pattern[0];
		}
	}
	else {
		if (pattern[0].x < pattern[1].x){
			returnPatterns[0] = pattern[0];
			returnPatterns[1] = pattern[2];
			returnPatterns[2] = pattern[1];
		}
		else{
			returnPatterns[0] = pattern[1];
			returnPatterns[1] = pattern[2];
			returnPatterns[2] = pattern[0];
		}
	}

	return returnPatterns;
}


Point QrDetectorMod::minCoord(vector<Point> v) {
	int minY = v[0].y;
	int minX = v[0].x;
	for (int i = 1; i < 4; i++) {
		if (v[i].y < minY) minY = v[i].y;
		if (v[i].x < minX) minX = v[i].x;

	}

	return Point(minX, minY);
}


Point QrDetectorMod::maxCoord(vector<Point> v) {
	int maxY = v[0].y;
	int maxX = v[0].x;
	for (int i = 1; i < 4; i++) {
		if (v[i].y > maxY) maxY = v[i].y;
		if (v[i].x > maxX) maxX = v[i].x;

	}

	return Point(maxX, maxY);
}


double QrDetectorMod::dist(Point v1, Point v2) {
	int dx = v1.x - v2.x;
	int dy = v1.y - v2.y;

	return sqrt(dx*dx + dy*dy);
}

double QrDetectorMod::dist(FP v1, FP v2) {
	int dx = v1.x - v2.x;
	int dy = v1.y - v2.y;

	return sqrt(dx*dx + dy*dy);
}

int QrDetectorMod::area(vector<Point> quad) {

	return (quad[0].x - quad[1].x) * (quad[1].y - quad[3].y);
}


bool QrDetectorMod::inOtherContour(vector<Point>* test) {

	Point testCenter = getCenter(*test);

	for (int i = 0; i < quadList.size(); i++) {

		if (dist(testCenter, quadList[i][4]) < 10) {
			if (area(*test) > area(quadList[i])) {
				(*test).push_back(testCenter);
				quadList[i] = *test;
				return true;
			}
			return true;
		}
	}

	(*test).push_back(testCenter);
	return false;
}

vector<int> QrDetectorMod::cross(Point a, Point b) {
	return{ a.y - b.y,
			b.x - a.x,
			a.x * b.y - a.y * b.x };
}

vector<int> QrDetectorMod::cross(vector<int> a, vector<int> b) {
	return{ a[1] * b[2] - a[2] * b[1],
			a[2] * b[0] - a[0] * b[2],
			a[0] * b[1] - a[1] * b[0] };
}

vector<int> QrDetectorMod::cross(FP a, FP b)
{
	return{ a.y - b.y,
			b.x - a.x,
			a.x * b.y - a.y * b.x };
}


Point QrDetectorMod::getCenter(vector<Point> quad) {

	vector<int> inters = cross(cross(quad[0], quad[3]), cross(quad[1], quad[2]));

	return Point(inters[0] / inters[2], inters[1] / inters[2]);
}
/*
Point QRdetector::intersectionPoint(vector<FinderPattern*> centers){

	vector<float> ab = cross(centers[0], centers[1]);
	vector<float> bc = cross(centers[1], centers[2]);

	vector<float> abParal = { ab[0], ab[1], (-ab[0] * centers[2]->getX() - ab[1] * centers[2]->getY()) };
	vector<float> bcParal = { bc[0], bc[1], -bc[0] * centers[0]->getX() - bc[1] * centers[0]->getY() };
	vector<float> inters = cross(abParal, bcParal);

	return Point(inters[0] / inters[2], inters[1] / inters[2]);
}*/


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

	double a = dist(pts[0], pts[2]);
	double b = dist(pts[0], pts[1]);
	double c = dist(pts[1], pts[3]);
	double d = dist(pts[2], pts[3]);
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
	for (int i = 0; i < 5; i++) {
		int count = stateCount[i];
		totalFinderSize += count;
		if (count == 0)
			return false;
	}

	if (totalFinderSize<7) {
		return false;
	}

	double moduleSize = ceil(totalFinderSize / 7.0);
	double maxVariance = moduleSize * 0.5;

	bool retVal = ((abs(moduleSize - stateCount[0])) <= maxVariance &&
		(abs(moduleSize - stateCount[1])) <= maxVariance &&
		(abs(3 * moduleSize - stateCount[2])) <= 3 * maxVariance &&
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
								module = stateCountTotal / 7.0;
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
		return false;
	}

	while (row >= 0 && img.at<uchar>(row, centerCol) > 128 && stateCount[1] <= blackSqrCount) {
		stateCount[1]++;
		row--;
	}
	if (row<0 || stateCount[1] > blackSqrCount) {
		return false;
	}

	while (row >= 0 && img.at<uchar>(row, centerCol) < 128 && stateCount[0] <= blackSqrCount) {
		stateCount[0]++;
		row--;
	}
	if (stateCount[0]>blackSqrCount) {
		return false;
	}

	// Now start moving down from the center
	row = startRow + 1;
	while (row<maxRow && img.at<uchar>(row, centerCol) < 128) {
		stateCount[2]++;
		row++;
	}
	if (row == maxRow) {
		return false;
	}

	while (row<maxRow && img.at<uchar>(row, centerCol) > 128 && stateCount[3]<blackSqrCount) {
		stateCount[3]++;
		row++;
	}
	if (row == maxRow || stateCount[3]>blackSqrCount) {
		return false;
	}

	while (row<maxRow && img.at<uchar>(row, centerCol) < 128 && stateCount[4] < blackSqrCount) {
		stateCount[4]++;
		row++;
	}
	if (stateCount[4] >= blackSqrCount) {
		return false;
	}

	return checkRatio(stateCount);
}

bool QrDetectorMod::firstHorizontalCheck(Mat img, int row) {

	int stateCount[5] = { 0 };
	int currentState = 0;

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
								module = stateCountTotal / 7.0;
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


	return false;
}



