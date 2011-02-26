#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"



class testApp : public ofBaseApp
{

	public:

		void setup();
		void update();
		void draw();
		void exit();

        int shareMemory();

		void drawPointCloud();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

        HANDLE hMapFile;
        LPCTSTR pBuf;

        ofxKinect           kinect;

		ofxCvColorImage		colorImg;

		ofxCvGrayscaleImage 	grayImage;
		ofxCvGrayscaleImage 	grayThresh;
		ofxCvGrayscaleImage 	grayThreshFar;

		ofxCvContourFinder 	contourFinder;

		bool                bShareMemory;
		bool				bThreshWithOpenCV;

		int 				nearThreshold;
		int					farThreshold;

		int					angle;

        unsigned char*      myPic;

};

#endif
