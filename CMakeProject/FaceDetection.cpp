/*
 * FaceDetection.cpp
 *
 *  Created on: Jun 20, 2019
 *      Author: esl45
 */

#include "FaceDetection.h"

FaceDetection::FaceDetection() {
	// TODO Auto-generated constructor stub
	memset(this->X, 0, sizeof(this->X));
	memset(this->Y, 0, sizeof(this->Y));


}

std::tuple<int, int> FaceDetection::Detect( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale)
{
    vector<Rect> faces, faces2;
    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY ); // Convert to Gray Scale
    double fx = 1 / scale;

    // Resize the Grayscale Image
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    // Detect faces of different sizes using cascade classifier
    cascade.detectMultiScale( smallImg, faces, 1.1,
                            2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    // Draw circles around the faces
    if(faces.size() > 0)
    {
        Rect r = faces[0];
        Mat smallImgROI;
        vector<Rect> nestedObjects;
        Point center;
        Scalar color = Scalar(255, 0, 0); // Color for Drawing tool
        int radius;

        double aspect_ratio = (double)r.width/r.height;
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {
            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);

            //radius = cvRound((r.width + r.height)*0.25*scale);
            //circle( img, center, radius, color, 3, 8, 0 );
        }
        smallImgROI = smallImg( r );

        ///std::cout << center.x << " " << center.y << std::endl;
        return std::make_tuple((int)center.x,(int)center.y);
    }

    // Show Processed Image with detected faces
    //imshow( "Face Detection", img );

    return std::make_tuple(0,0);
}


void FaceDetection::QueueXY(std::tuple<int, int> XY){

	int newX, newY;
	int newArrayX[DetectAverage];
	int newArrayY[DetectAverage];

	tie(newX, newY) = XY;

	//Don't average on no faces
	if((newX == 0) && (newY == 0)){
		return;
	}

	for(int i = 0; i < DetectAverage-1; i++){
		newArrayX[i+1] = this->X[i];
		newArrayY[i+1] = this->Y[i];

	}

	newArrayX[0] = newX;
	newArrayY[0] = newY;

	for(int i = 0; i < DetectAverage; i++){
		this->X[i] = newArrayX[i];
		this->Y[i] = newArrayY[i];
	}

	return;
}

std::tuple<int, int> FaceDetection::ReturnXY(){
	int outX = 0;
	int outY = 0;

	for(int i = 0; i < DetectAverage; i++){
		outX = outX + this->X[i];
		outY = outY + this->Y[i];
	}

	outX = outX / DetectAverage;
	outY = outY / DetectAverage;

	return std::make_tuple(outX, outY);
}


