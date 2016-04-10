#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "TestModule.h"
#include "QrDetectorMod.h"

using namespace cv;
using namespace std;


int main()
{
	const int n = 20;
	TestModule test = TestModule(n);
	test.startTest();
	return 0;
}


