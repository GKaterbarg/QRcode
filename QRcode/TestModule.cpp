#include "TestModule.h"


TestModule::TestModule(int numImages){
	this -> numImages = numImages;
}


void TestModule::startTest() {
	vector<vector<Point>> realCoords = readRealCoords();
	int n = numImages;
	int size;
	if ((size = realCoords.size()) < numImages) n = size;
	QrDetectorMod qrDet = QrDetectorMod();
	for (int i = 0; i < n; i++) {
		string path = "Test/" + to_string(i) + ".jpg";
		img = imread(path);

		qrDet.setImage(img);
		vector<FP> fps = qrDet.find();
		if (fps.size() > 2) {
			printf("Image %i True\n", i);
			float s = getAreaRect(fps);
		}
		else printf("Image %i False\n", i);

		for each(FP fp in fps){
			
				circle(img, Point(fp.x, fp.y), 5, Scalar(0, 0, 255), -1);

		}

		for each(Point pt in realCoords[i]) {
			circle(img, pt, 5, Scalar(255, 0, 0), -1);
		}

		waitKey(500);
		imshow("Original", img);

	}
}


vector<vector<Point>> TestModule::readRealCoords() {
	vector<vector<Point>>base;
	string line;
	ifstream file("Test/Input.txt");

	while (file) {
		getline(file, line);
		vector<Point> coords;
		for (int i = 0; i < line.length(); i++) {
			string x, y;
			int ispace = line.find(" ");
			if (ispace == -1) ispace = line.length();
			bool f = false;

			for (int j = 0; j < ispace; j++) {
				if (line[j] == ';'){
					f = true;
					continue;
				}
				if (!f) x = x + line[j];
				else y = y + line[j];
			}

			string tmp;
			for (int k = ispace + 1; k < line.length(); k++) {
				tmp = tmp + line[k];
			}
			line = tmp;

			coords.push_back(Point(stof(x), stof(y)));
		}
		if (coords.size() != 0) base.push_back(coords);
	}

	return base;
}


float TestModule::dist(Point v1, Point v2) {
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;

	return sqrt(dx*dx + dy*dy);
}


float TestModule::getArea(vector<Point> coords) {
	float a = dist(coords[0], coords[1]);
	float b = dist(coords[2], coords[3]);
	float c = dist(coords[0], coords[2]);
	float d = dist(coords[1], coords[3]);
	float p = (a + b + c + d) / 2;

	return sqrt((p - a)*(p - b)*(p - c)*(p - d));
}


float TestModule::getAreaRect(vector<FP> qrCode) {
	float maxY = 0.0;
	float minY = img.rows;
	float maxX = 0.0;
	float minX = img.cols;
	int x, y;
	int interval = 4 * qrCode[0].module;
	for (int i = 0; i < 4; i++) {
		if ((y = qrCode[i].y) < minY) minY = y;
		if ((y = qrCode[i].y) > maxY) maxY = y;
		if ((x = qrCode[i].x) < minX) minX = x;
		if ((x = qrCode[i].x) > maxX) maxX = x;
	}

	rectangle(img, Point(minX - interval, maxY + interval), Point(maxX + interval, minY - interval), Scalar(0, 0, 255), 3);

	return (maxX - minX + 2 * interval) * (maxY - minY + 2 * interval);
}
