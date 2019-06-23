/*
 * FaceDetection.h
 *
 *  Created on: Jun 20, 2019
 *      Author: esl45
 */
//#include "/usr/include/opencv2/objdetect/objdetect.hpp"
//#include "/usr/include/opencv2/highgui/highgui.hpp"
//#include "/usr/include/opencv2/imgproc/imgproc.hpp"
#include "/usr/local/include/opencv2/objdetect.hpp"
#include "/usr/local/include/opencv2/highgui.hpp"
#include "/usr/local/include/opencv2/imgproc.hpp"

#include <iostream>
#include <tuple>


using namespace std;
using namespace cv;

#ifndef FACEDETECTION_H_
#define FACEDETECTION_H_

#define DetectAverage 10

class FaceDetection {
	int X[DetectAverage];
	int Y[DetectAverage];
public:
	FaceDetection();

	//Detect Face
	std::tuple<int, int> Detect( Mat& img, CascadeClassifier& cascade,
	                CascadeClassifier& nestedCascade, double scale );

	//Add Face to Queue
	void QueueXY(std::tuple<int, int> XY);

	//Average Faces
	std::tuple<int, int> ReturnXY();
};

#endif /* FACEDETECTION_H_ */
