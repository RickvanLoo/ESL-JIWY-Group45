/*
 * AngleConvert.h
 *
 *  Created on: Jun 20, 2019
 *      Author: esl45
 */
#include <stdlib.h>
#include <iostream>


#ifndef ANGLECONVERT_H_
#define ANGLECONVERT_H_

#define VisionXValue 8
#define VisionYValue 8

class AngleConvert {
public:
	AngleConvert(int minCount, int maxCount, int maxX, int maxY);
	void SetCount(int Count);
	int Convert(int X);
private:
	int CurrentCount;
	int minCount;
	int TotalPan;
	int maxCount;
	int maxX;
};

#endif /* ANGLECONVERT_H_ */
