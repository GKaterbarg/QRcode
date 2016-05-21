#include "QrDetectorMod.h"

QrDetectorMod::QrDetectorMod() {

}

void QrDetectorMod::setImage(Mat image) {

	this->image = image;
	quadList = vector<vector<Point>>();
	module = 0.0;
}


// Main execute funcion---------------------------------------------------------------------------------
vector<FP> QrDetectorMod::find() {
	Mat gray = Mat(image.rows, image.cols, CV_8UC1);
	Mat edges(image.size(), CV_MAKETYPE(image.depth(), 1));
	cvtColor(image, gray, CV_BGR2GRAY);
	Canny(gray, edges, 100, 200, 3);

	vector<vector<Point>> contours;
	vector<Point> approx;
	findContours(edges, contours, RETR_LIST, CHAIN_APPROX_NONE); //TODO: Has trik this func ?

	for (int i = 0; i < contours.size(); i++)
	{

		approx = approximate(contours[i]);
		//for each (Point p in approx) circle(image, Point(p.x, p.y), 1, Scalar(0, 0, 255), -1); // for degug
		if (approx.size() == 4){
			//drawContours(image, contours, i, Scalar(255, 0, 0), CV_FILLED); //for debug
			if (isQuad(&approx) && abs(area(approx)) > 10){
				if (!inOtherContour(&approx)){
					quadList.push_back(vector<Point>(approx));
				}
			}
		}
	}

	if (quadList.size() < 2){
		return vector<FP>();
	}

	vector<FP> fps;
	for each(vector<Point> quad in quadList){

		Point min = minCoord(quad);
		Point max = maxCoord(quad);
		int x = min.x - 0.7*(max.x - min.x),
			y = min.y - 0.7*(max.y - min.y);
		if (x < 0) x = 0; if (y < 0) y = 0;
			
		int	w = 2.8 * (max.x - min.x),
			h = 2.8 * (max.y - min.y);
		if (h > 0.5*image.rows || w > 0.5*image.cols) continue;
		if (x + w > gray.cols) w = gray.cols - x - 1;
		if (h + y > gray.rows) h = gray.rows - y - 1;

		Mat partImg = gray(Rect(x, y, w, h));
		threshold(partImg, partImg, 128, 255, THRESH_OTSU); // TODO: Has trik this func?
		int dif = quad[4].y - y;
		if (dif >= partImg.rows || dif <= 0) continue;
		if (firstHorizontalCheck(partImg, dif)) {
			fps.push_back(FP(quad[4].x, quad[4].y, module));
			}
		else {
			if (horizontalCheck(partImg)) {
				fps.push_back(FP(quad[4].x, quad[4].y, module));				}
			}
			//imshow("Parts", partImg);//for debug
			//waitKey(1200);//for debug
		}

		Point intersPt;
		if (fps.size() == 3){
			fps = orderBestPatterns(fps);
			intersPt = intersectionPoint(fps);
			fps.push_back(FP(intersPt.x, intersPt.y, -1));
		}

		return fps;
}


// Contour filtration --------------------------------------------------------------------------------------------------------
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
	if (testCenter.x == NULL){
		return true;
	}
		for (int i = 0; i < quadList.size(); i++) {

			if (dist(testCenter, quadList[i][4]) < 5) {
				if (area(*test) < area(quadList[i])) {
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
	if (inters[2] != 0){
		return Point(inters[0] / inters[2], inters[1] / inters[2]);
	}
	else{
		return Point(NULL,NULL);
	}
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

	double a = dist(pts[0], pts[2]);
	double b = dist(pts[0], pts[1]);
	double c = dist(pts[1], pts[3]);
	double d = dist(pts[2], pts[3]);
	if (a < c) swap(a, c);
	if (b < d) swap(b, d);

	if (c / a < 0.9 || d / b < 0.9) return false;
	if (b / a < 0.5 || a / b < 0.6) return false;
	if (d / a < 0.5 || a / d < 0.6) return false;

	return true;
}


// Binary image analysis---------------------------------------------------------------------------------------
bool QrDetectorMod::checkRatio(int stateCount[]) {
	int totalFinderSize = 0;
	for (int i = 0; i < 5; i++) {
		int count = stateCount[i];
		totalFinderSize += count;
		if (count == 0)
			return false;
	}

	if (totalFinderSize < 7) {
		return false;
	}

	double moduleSize = ceil(totalFinderSize / 7.0);
	double maxVariance = moduleSize * 0.5;

	bool retVal = ((abs(moduleSize - stateCount[0])) <= maxVariance &&
		(abs(moduleSize - stateCount[1])) <= maxVariance &&
		(abs(3 * moduleSize - stateCount[2])) < 3 * maxVariance &&
		(abs(moduleSize - stateCount[3])) <= maxVariance &&
		(abs(moduleSize - stateCount[4])) <= maxVariance);

	return retVal;
}

float QrDetectorMod::centerFromEnd(int stateCount[], int end) {
	return (end - stateCount[4] - stateCount[3]) - stateCount[2] / 2.0;
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


// Approximation--------------------------------------------------------------------------------------------------------
int QrDetectorMod::removeExtraPoints(vector<Point>* dst, double eps){
	int count = (*dst).size();
	int new_count = count;
	int pos = count - 1;
	Point start_pt = (*dst)[pos];
	if (++pos >= count) pos = 0;
	int wpos = pos;
	Point pt = (*dst)[pos];
	if (++pos >= count) pos = 0;

	for (int i = 0; i < count && new_count > 2; i++)
	{
		double dx, dy, dist, successive_inner_product;
		Point end_pt = (*dst)[pos];
		if (++pos >= count) pos = 0;

		dx = end_pt.x - start_pt.x;
		dy = end_pt.y - start_pt.y;
		dist = fabs((pt.x - start_pt.x)*dy - (pt.y - start_pt.y)*dx);
		successive_inner_product = (pt.x - start_pt.x) * (end_pt.x - pt.x) +
			(pt.y - start_pt.y) * (end_pt.y - pt.y);

		if (dist * dist <= 0.5*eps*(dx*dx + dy*dy) && dx != 0 && dy != 0 &&
			successive_inner_product >= 0)
		{
			new_count--;
			(*dst)[wpos] = start_pt = end_pt;
			if (++wpos >= count) wpos = 0;
			Point pt = (*dst)[pos];
			if (++pos >= count) pos = 0;
			i++;
			continue;
		}
		(*dst)[wpos] = start_pt = pt;
		if (++wpos >= count) wpos = 0;
		pt = end_pt;
	}

	return new_count;
}


vector<Point> QrDetectorMod::approximate(vector<Point> contour){
	if (contour.size() > 2){
		pair<int, int> furthestPts = findFurthestPts(contour);
		vector<Point> clockwise;
		vector<Point> counterclockwise;
		double eps = contourLength(contour)*0.04;

		for (int i = furthestPts.first; i < furthestPts.second + 1; i++){
			clockwise.push_back(contour[i]);
		}
		for (int i = furthestPts.second; i < contour.size(); i++){
			counterclockwise.push_back(contour[i]);
		}
		for (int i = 0; i < furthestPts.first + 1; i++){
			counterclockwise.push_back(contour[i]);
		}

		vector<Point> approx = simplifyWithRDP(clockwise, 0, clockwise.size() - 1, eps);
		vector<Point> approx2 = simplifyWithRDP(counterclockwise, 0, counterclockwise.size() - 1, eps);
		approx.insert(approx.end(), approx2.begin(), approx2.end());
		int newc = 0;
		vector<Point> approx3;
		if (approx.size() > 2){
			newc = removeExtraPoints(&approx, eps);
			vector<Point> newap(approx.begin(), approx.begin() + newc - 1);
			return newap;
		}
		else{

			return approx;
		}
	}
	else{

		return contour;
	}
}


double QrDetectorMod::contourLength(vector<Point> contour){
	if (contour.size() <= 1)
		return 0.0;
	double perimeter = 0.0;

	for (int i = 0; i < contour.size() - 1; i++) {
		perimeter += dist(contour[i], contour[i + 1]);
	}

	return perimeter + dist(contour[0], contour[contour.size() - 1]);
}

double QrDetectorMod::pointLineDistance(Point point, Point start, Point end) {
	if (start.x == end.x && start.y == end.y) {
		return dist(point, start);
	}
	double n = abs((end.x - start.x) * (start.y - point.y) - (start.x - point.x) * (end.y - start.y));
	double d = sqrt((end.x - start.x) * (end.x - start.x) + (end.y - start.y) * (end.y - start.y));

	return n / d;
}

pair<int, int> QrDetectorMod::findFurthestPts(vector<Point> pts) {
	double mdist = -1.0;
	int fst, lst;
	for (int i = 0; i < pts.size() - 1; i++){
		for (int j = i + 1; j < pts.size(); j++){
			double d = dist(pts[i], pts[j]);
			if (d > mdist){
				mdist = d;
				fst = i;
				lst = j;
			}
		}
	}

	return make_pair(fst, lst);
}

vector<Point> QrDetectorMod::simplifyWithRDP(vector<Point>& points, int startIndex, int lastIndex, double epsilon) {
	if (points.size() < 3 ){  //base case 1
		return points;
	}

	int index = startIndex;
	double dMax = -1.0; 

	//distance calculation
	for (int i = index + 1; i < lastIndex; i++){ 
		double d = pointLineDistance(points[i], points[startIndex], points[lastIndex]);
		if (d > dMax){
			dMax = d;
			index = i;
		}
	}

	if (dMax > epsilon){

		vector<Point> r1 = simplifyWithRDP(points, startIndex, index, epsilon);
		vector<Point> r2 = simplifyWithRDP(points, index, lastIndex, epsilon);

		//Concat simplified path1 and path2 together
		vector<Point> rs(r1);
		rs.pop_back();
		rs.insert(rs.end(), r2.begin(), r2.end());
		return rs;
	}
	else { //base case 2, all points between are to be removed.
		vector<Point> r(1, points[startIndex]);
		r.push_back(points[lastIndex]);
		return r;
	}
}


// Final work with FPs-----------------------------------------------------------------------------------------------------------
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

 





