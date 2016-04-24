#ifndef __FP
#define __FP

class FP
{
public:
	FP::FP(int x, int y, double module) {
		this->x = x;
		this->y = y;
		this->module = module;
	}
	FP::FP() {}

public:
	int x;
	int y;
	double module;
};
#endif
