#include "TestModule.h"


using namespace cv;
using namespace std;


TestModule::TestModule(int numImages){
	this -> numImages = numImages;
}


void TestModule::startTest()
{
	vector<vector<Point>> realCoords = readRealCoords();
	QRdetector qrDet = QRdetector();
	int n = numImages;
	int size;
	if ((size = realCoords.size()) < numImages) n = size;

	for (int i = 0; i < n; i++) {
		string path = "Test/" + to_string(i) + ".jpg";
		img = imread(path);

		Mat imgBW = Mat(img.rows, img.cols, CV_8UC1);
		cvtColor(img, imgBW, CV_BGR2GRAY);
		threshold(imgBW, imgBW, 128, 255, THRESH_OTSU);

		qrDet.setImg(imgBW);
		vector<FinderPattern*> qrCode = qrDet.find();

		float s;
		Point intersectionPt;
		if (qrCode.size() == 3) {
			intersectionPt = qrDet.intersectionPoint(qrCode);
			s = getAreaRect(qrCode, intersectionPt);
		}
		else s = -1.0;

		if (s > 0.7*getArea(realCoords[i])) printf("Test image # %i: True\n", i);
		else printf("Test image # %i: False\n", i);

		drawPoints(realCoords[i], qrCode, intersectionPt);
	}
}


void TestModule::drawPoints(vector<Point> realCoords, vector<FinderPattern*> qrCode, Point intersPt) {

	for each (FinderPattern* fp in qrCode) {
		if (fp != NULL) circle(img, Point(fp->getX(), fp->getY()), 5, Scalar(0, 0, 255), -1);
	}

	for each(Point pt in realCoords) {
		circle(img, pt, 5, Scalar(255, 0, 0), -1);
	}

	imshow("Original", img);
	waitKey(1500);
}


vector<vector<Point>> TestModule::readRealCoords()
{
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


float TestModule::dist(Point v1, Point v2)
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;

	return sqrt(dx*dx + dy*dy);
}


float TestModule::getArea(vector<Point> coords)
{
	float a = dist(coords[0], coords[1]);
	float b = dist(coords[2], coords[3]);
	float c = dist(coords[0], coords[2]);
	float d = dist(coords[1], coords[3]);
	float p = (a + b + c + d) / 2;

	return sqrt((p - a)*(p - b)*(p - c)*(p - d));
}


float TestModule::getAreaRect(vector<FinderPattern*> qrCode, Point intersPt)
{
	float maxY = 0.0;
	float minY = img.rows;
	float maxX = 0.0;
	float minX = img.cols;
	float x, y;
	int interval = 4 * ceil(qrCode[1]->getEstimatedModuleSize());
	for (int i = 0; i < 3; i++) {
		if ((y = qrCode[i]->getY()) < minY) minY = y;
		if ((y = qrCode[i]->getY()) > maxY) maxY = y;
		if ((x = qrCode[i]->getX()) < minX) minX = x;
		if ((x = qrCode[i]->getX()) > maxX) maxX = x;
	}
	if (maxY < intersPt.y) maxY = intersPt.y;
	if (minY > intersPt.y) minY = intersPt.y;
	if (maxX < intersPt.x) maxX = intersPt.x;
	if (minX > intersPt.x) minX = intersPt.x;

	rectangle(img, Point(minX - interval, maxY + interval), Point(maxX + interval, minY - interval), Scalar(0, 0, 255), 3);

	return (maxX - minX + 2 * interval) * (maxY - minY + 2 * interval);
}