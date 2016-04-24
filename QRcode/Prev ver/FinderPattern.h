#ifndef __FINDER_PATTERN
#define __FINDER_PATTERN

#include <cmath>

class FinderPattern {
private:
    float estimatedModuleSize;
    int count;
    float posX, posY;

public:
    FinderPattern(float posX, float posY, float estimatedModuleSize);
    FinderPattern(float posX, float posY, float estimatedModuleSize, int count);
    float getEstimatedModuleSize();
    int getCount();
    void incrementCount();
    bool aboutEquals(float, float, float);
    FinderPattern* combineEstimate(float, float, float);
    float getX();
    float getY();
};

#endif 
