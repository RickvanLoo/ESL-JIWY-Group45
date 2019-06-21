/*
 * AngleConvert.cpp
 *
 *  Created on: Jun 20, 2019
 *      Author: esl45
 */

#include "AngleConvert.h"

AngleConvert::AngleConvert(int minCount, int maxCount, int maxX, int maxY) {
	// TODO Auto-generated constructor stub
	this->minCount = minCount;
	this->maxCount = maxCount;
	this->maxX = maxX;
	this->CurrentCount = 0;

	int TotalCount = abs(minCount) + abs(maxCount);
	this->TotalPan = TotalCount / VisionXValue;
}

void AngleConvert::SetCount(int Count){
	this->CurrentCount = Count;
}

int AngleConvert::Convert(int X){
	float A = (float)this->TotalPan/(float)this->maxX;
	float Out = (A * X) + (this->CurrentCount - (this->TotalPan/2));

	return (int)Out;
}


