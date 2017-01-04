#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
/* #include "ofxReact.h" */

class testApp : public ofBaseApp {
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased (int key);
	
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();

	float CutOff;
	
	/* ofxReact React(); */
	void audioIn(float * input, int bufferSize, int nChannels);
	ofSoundStream soundStream;
	int bufferCounter;
	float smoothedVol;
	float scaledVol;
	float formerVol;
	vector <float> left;
	int bufferSize;

  ofEasyCam cam;

};

#endif

