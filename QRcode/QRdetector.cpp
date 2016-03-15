#include "QRdetector.h"

QRdetector::QRdetector() {
	this->centers = vector<FinderPattern*>();
}

void QRdetector::setImg(Mat img) {
	this->img = img;
}

vector<FinderPattern*> QRdetector::find() {

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
							bool confirmed = checkPossibleCenter(stateCount, row, col);
							if (confirmed) {
								skipRows = 1;
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
	// end looping rows

	for (unsigned int l = 0; l<centers.size(); l++) {
		printf("(%f, %f)\n", centers[l]->getX(), centers[l]->getY());
	}

	vector<FinderPattern*> patternInfo = identifyBestPatterns();
	if (patternInfo.size() > 2) 
		patternInfo = orderBestPatterns(patternInfo);
	return patternInfo;

}


vector<FinderPattern*> QRdetector::identifyBestPatterns() {
	int initialSize = centers.size();
	if (initialSize > 3) {
		float combinedModuleSize = 0.0f;
		float sq = 0.0f;

		for (unsigned int index = 0; index<centers.size(); index++) {
			float size = centers[index]->getEstimatedModuleSize();
			combinedModuleSize += size;
			sq += size*size;
		}
		float avg = combinedModuleSize / (float)initialSize;
		float stdDev = (float)sqrt(sq / initialSize - avg*avg);
		float limit = 0.2f*avg;
		if (stdDev>limit)
			limit = stdDev;

		for (unsigned int l = 0; l < centers.size(); l++) {
			for (unsigned int k = 0; k<centers.size() - 1; k++) {
				if (fabs(centers[k]->getEstimatedModuleSize() - avg) > fabs(centers[k + 1]->getEstimatedModuleSize() - avg)) {
					FinderPattern *temp = centers[k];
					centers.erase(centers.begin() + k);

					centers.insert(centers.begin() + k + 1, temp);
					k--;
				}
			}
		}

		for (unsigned int l = 0; l<centers.size() && centers.size() > 3; l++) {
			if (fabs(avg - centers[l]->getEstimatedModuleSize()) > limit) {
				centers.erase(centers.begin() + l);
				l--;
			}
		}
	}
	vector<FinderPattern*> patterns;
	for (int i = 0; i < centers.size(); i++){
		patterns.push_back(centers[i]);
	}

	return patterns;
}


// 0 and 2 elements of returned vec locate on hypotenuse, 0 - left FP, 2 - right FP 
vector<FinderPattern*> QRdetector::orderBestPatterns(vector<FinderPattern*> pattern) {
	float distance01 = sqrt((pattern[0]->getX() - pattern[1]->getX())*(pattern[0]->getX() - pattern[1]->getX()) +
		(pattern[0]->getY() - pattern[1]->getY())*(pattern[0]->getY() - pattern[1]->getY()));
	float distance12 = sqrt((pattern[1]->getX() - pattern[2]->getX())*(pattern[1]->getX() - pattern[2]->getX()) +
		(pattern[1]->getY() - pattern[2]->getY())*(pattern[1]->getY() - pattern[2]->getY()));
	float distance02 = sqrt((pattern[0]->getX() - pattern[2]->getX())*(pattern[0]->getX() - pattern[2]->getX()) +
		(pattern[0]->getY() - pattern[2]->getY())*(pattern[0]->getY() - pattern[2]->getY()));

	vector<FinderPattern*> returnPatterns(3);

	if (distance12 >= distance01 && distance12 >= distance02) {
		if (pattern[1]->getX() < pattern[2]->getX()){
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
		if (pattern[0]->getX() < pattern[2]->getX()){
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
		if (pattern[0]->getX() < pattern[1]->getX()){
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


bool QRdetector::checkRatio(int stateCount[]) {
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


float QRdetector::centerFromEnd(int stateCount[], int end) {
	return (float)(end - stateCount[4] - stateCount[3]) - stateCount[2] / 2.0f;
}

float QRdetector::crossCheckVertical(int startRow, int centerCol, int blackSqrCount, int originalStateCountTotal) {
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

	return checkRatio(stateCount) ? centerFromEnd(stateCount, row) : (NAN);
}


float QRdetector::crossCheckHorizontal(int startCol, int centerRow, int blackSqrCount, int originalStateCountTotal) {
	int maxCol = img.cols;
	int stateCount[5] = { 0 };

	int col = startCol;
	while (col >= 0 && img.at<uchar>(centerRow, col)<128) {
		stateCount[2]++;
		col--;
	}
	if (col<0) {
		return NAN;
	}

	while (col >= 0 && img.at<uchar>(centerRow, col)>128 && stateCount[1] <= blackSqrCount) {
		stateCount[1]++;
		col--;
	}
	if (col<0 || stateCount[1]>blackSqrCount) {
		return NAN;
	}

	while (col >= 0 && img.at<uchar>(centerRow, col)<128 && stateCount[0] <= blackSqrCount) {
		stateCount[0]++;
		col--;
	}
	if (col<0 || stateCount[0]>blackSqrCount) {
		return NAN;
	}

	col = startCol + 1;
	while (col<maxCol && img.at<uchar>(centerRow, col)<128) {
		stateCount[2]++;
		col++;
	}
	if (col == maxCol) {
		return NAN;
	}

	while (col<maxCol && img.at<uchar>(centerRow, col)>128 && stateCount[3] <= blackSqrCount) {
		stateCount[3]++;
		col++;
	}
	if (col == maxCol || stateCount[3]>blackSqrCount) {
		return NAN;
	}

	while (col<maxCol && img.at<uchar>(centerRow, col)<128 && stateCount[4] <= blackSqrCount) {
		stateCount[4]++;
		col++;
	}
	if (col == maxCol || stateCount[4]>blackSqrCount) {
		return NAN;
	}

	return checkRatio(stateCount) ? centerFromEnd(stateCount, col) : NAN;
}


bool QRdetector::checkPossibleCenter(int stateCount[], int row, int col) {
	int stateCountTotal = stateCount[0] + stateCount[1] + stateCount[2] + stateCount[3] + stateCount[4];
	float centerCol = centerFromEnd(stateCount,col);
	float centerRow = crossCheckVertical(row, (int)centerCol, stateCount[2], stateCountTotal);

	if (!isnan(centerRow)) {

		centerCol = crossCheckHorizontal((int)centerCol, (int)centerRow, stateCount[2], stateCountTotal);

		if (!isnan(centerCol)) {
			float estimatedModuleSize = (float)stateCountTotal / 7.0f;
			bool found = false;
			for (unsigned int index = 0; index<this->centers.size(); index++) {
				FinderPattern *center = this->centers[index];
				if (center->aboutEquals(estimatedModuleSize, centerRow, centerCol)) {
					this->centers[index] = center->combineEstimate(centerRow, centerCol, estimatedModuleSize);
					found = true;
					break;
				}
			}

			if (!found) {
				FinderPattern *newCenter = new FinderPattern(centerCol, centerRow, estimatedModuleSize);
				printf("Created new center: (%f, %f)\n", newCenter->getX(), newCenter->getY());
				centers.push_back(newCenter);
			}

			return true;
		}
	}
	printf("Returning false\n");
	return false;
}
