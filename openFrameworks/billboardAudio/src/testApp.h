#pragma once
#include "ofMain.h"
#include "ofVbo.h"
//#include "ofxReact.h"
//#include "ofxAudioFeatures.h" //add timeline include

#define NUM_BILLBOARDS 5000

typedef struct {

	float 	x;
	float 	y;
	bool 	bBeingDragged;
	bool 	bOver;
	float 	radius;
	
}draggableVertex;


class testApp : public ofBaseApp {
 public:
  void setup();
  void update();
  void draw();

  //  ofxReact React();

		
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y);
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);
		
  ofVec3f cameraRotation;
  float zoom, zoomTarget;
		
  // billboard particles
  float billboardSizeTarget[NUM_BILLBOARDS];
		
  ofShader billboardShader;
  ofImage texture;
		
  ofVboMesh billboards;
  ofVec3f billboardVels[NUM_BILLBOARDS];

  ofSoundPlayer my_sound;
  int IfSong;

  ofSoundStream soundStream;
  void audioIn(float * input, int bufferSize, int nChannels); 
  int bufferCounter;
  int IfStream;
  float smoothedVol;
  float scaledVol;
  vector <float> left;
  int bufferSize;
  float StreamAmp;

  int nBandsToGet;
  float *fftSmoothed;
  float SongAmp;

  int nCurveVertices;
  draggableVertex curveVertices[7];

  ofEasyCam cam;
  
};
