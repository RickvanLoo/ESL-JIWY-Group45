// CPP program to detects face in a video

// Include required header files from OpenCV directory
//#include "/usr/include/opencv2/objdetect/objdetect.hpp"
//#include "/usr/include/opencv2/highgui/highgui.hpp"
//#include "/usr/include/opencv2/imgproc/imgproc.hpp"
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "FaceDetection.h"
#include "AngleConvert.h"
#include "SerialConnection.h"
#include <iostream>
#include <tuple>

using namespace std;
using namespace cv;

string cascadeName, nestedCascadeName;

int main( int argc, const char** argv )
{
	int MinPan = 0;
	int MaxPan = 0;

	SerialConnection Serial("/dev/ttyO0");
	if(Serial.Connect()){
		return 0;
	}

	if(Serial.Config()){
		return 0;
	}

	if(Serial.Reset()){
		return 0;
	}

	if(Serial.HomingPan()){
		return 0;
	}

	MinPan = Serial.MinCountPan();
	MinPan = Serial.MaxCountPan();

    // VideoCapture class for playing video for which faces to be detected
    VideoCapture capture;
    Mat frame, image;

    // PreDefined trained XML classifiers with facial features
    CascadeClassifier cascade, nestedCascade;
    double scale=1;

    // Load classifiers from "opencv/data/haarcascades" directory
    nestedCascade.load( "/home/esl45/Documents/opencv/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml" ) ;

    // Change path before execution
    cascade.load( "/home/esl45/Documents/opencv/data/haarcascades/haarcascade_frontalcatface.xml" ) ;

    // Start Video..1) 0 for WebCam 2) "Path to Video" for a Local Video
    capture.open(0);
    if( capture.isOpened() )
    {

        int X;
        int Y;

        std::tuple<int,int> XY;


        Point center;
        Scalar colorA = Scalar(0, 255, 0); // Color for Drawing tool
        Scalar colorB = Scalar(255, 0, 0); // Color for Drawing tool


        FaceDetection FaceDet;


        // Capture frames from video and detect faces
        cout << "Face Detection Started...." << endl;
        while(1)
        {
            capture >> frame;
            if( frame.empty() )
                break;
            Mat frame1 = frame.clone();

            AngleConvert PanConv(MinPan, MaxPan, frame1.cols, frame1.rows);


            XY = FaceDet.Detect(frame1, cascade, nestedCascade, scale );
            FaceDet.QueueXY(XY);
            tie(X,Y) = FaceDet.ReturnXY();

            std::cout << X << " " << Y << std::endl;
            //center.x = X;
            //center.y = Y;
            //( frame1, center, 30, colorA, 3, 8, 0 );


            //Point ZeroPoint;
            //ZeroPoint.x = frame1.cols/2;
            //ZeroPoint.y = frame1.rows/2;

            //circle( frame1, ZeroPoint, 5, colorB, 3, 8, 0 );


            PanConv.SetCount(Serial.GetPan());
            int OutCount = PanConv.Convert(X);
            std::cout << "Count: " << OutCount << std::endl;

            if(Serial.SetPan(OutCount)){
            	return 0;
            }


            //imshow( "Face Detection", frame1 );


            char c = (char)waitKey(10);

            // Press q to exit from window
            if( c == 27 || c == 'q' || c == 'Q' )
                break;
        }
    }
    else
        cout<<"Could not Open Camera";
    return 0;
}






