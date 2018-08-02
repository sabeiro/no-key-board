#ifndef TEST_APP_H
#define TEST_APP_H
#pragma once
#define NPhTot 10
#define NNoteTot 48
#define NHarm 16
#define NRegister 6
#define HOST "localhost"
#define PORT 57110 //57110 is SuperCollider default port

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxMidi.h"
#include "ofxOsc.h"
#include "ofxOscParameterSync.h"
#include "ofxGui.h"
#include <iostream>
#include <fstream>

class keyParam{
 public:
  keyParam();
  void calcDependencies();
  void readConf(char *FName);
  void writeConf(char *FName);
  float attack(float time);
  float decay(float time);
  float release(float time);
  string reportString();
  float vNote[NNoteTot];
  bool IsSine;
  bool IsTestSine;
  bool IsHarm;
  bool IsSquare;
  bool IsQuant;
  bool IsContour;
  bool IsAudio;
  bool IsVideo;
  bool IsMidi;
  bool IsInvert;
  bool IsOsc;
  bool IsWhite;
  bool IsEnvelope;
  bool IsFollowing;
  bool IsMirror;
  bool IsPortamento;
  int NReg;
  int ColTh;
  int FrameRate;
  int bufferSize;
  int sampleRate;
  int vw;
  int vh;
  int midiChannel;
  int midiPort;
  unsigned int midiProg;
  int PitchDist;
  float timbro[16];
  float SynthTime[4];
  float SynthVol[4];
  float AudioGain;
  float MidiGain;
  float SliceFactU;
  float SliceFactD;
  float ColInit;
  float dt;
  ofParameter<float> size;
  ofParameter<int> NCorrW;
  ofParameter<int> NCorrH;
  ofParameter<int> SliceD;
  ofParameter<int> SliceU;
  ofParameter<int> NoteMin;
  ofParameter<int> NNote;
  ofParameter<bool> check;
  ofParameter<float> ContMinA;
  ofParameter<float> ContMaxA;
  ofParameterGroup parameters;
  ofParameter<ofColor> colTh;
};

class keyFinger{
 public:
  keyFinger();
  void setId(int i);
  void assignNote(keyParam Par,cv::Point2f Cent,float area);
  void evolve(keyParam Par);
  void sendOsc();
  int findHistory(int *NewPos,int NPos,keyParam Par);
  string reportString(keyParam Par);
  ofxOscSender sender;
  int id;
  int NoteQuant;
  int OldNoteQuant;
  int State;
  int NoteCounter;
  int midiVel;
  float volume;
  float pitch;
  float control;
  float NoteTime;
  float phase;
  float area;
  float frequency;
  float sPan;
  float phaseAdder;
  float phaseAdderTarget;
  float x;
  float y;
};

class testApp : public ofBaseApp{
 public:
  void setup();
  void update();
  void draw();
  void exit(void);
		
  void keyPressed(int key);
  void keyReleased(int key);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void windowResized(int w, int h);
  void dragEvent(ofDragInfo dragInfo);
  void gotMessage(ofMessage msg);		


  ofxOscParameterSync sync;

  //the view window is defined by 3 corners
  ofVec3f windowTopLeft;
  ofVec3f windowBottomLeft;
  ofVec3f windowBottomRight;
  ofCamera headTrackedCamera;
  ofEasyCam previewCamera;
	
  bool usePreview;
  int NPhase;
  float windowWidth;
  float windowHeight;
  float viewerDistance;
	
  //deque<ofPoint> headPositionHistory;
  //-------------------audio------------------------
  ofSoundStream soundStream;
  float 	pan;
  int		sampleRate;
  void audioOut(float * input, int bufferSize, int nChannels);
  void envelope(int *NotePos,cv::Point2f *center,float *area,int NPhOld);
  void notEnvelope(cv::Point2f *center,float *area);
  void clearSample(int bufferSize);
  void addWhite(int bufferSize);
  void addSine(int bufferSize);
  void addTestSine(int bufferSize);
  void addSquare(int bufferSize);
  void addHarm(int bufferSize);
  void followSignal(int buffersize);
  //-------------------draw--------------------------
  void displayFinger();
  ofxFloatSlider radius;
  ofxPanel gui;
  //----------------------cv--------------------------
  ofVideoGrabber video;
  ofxCv::ContourFinder contourFinder;
  void getCenterCont(char unsigned *ImPix,int vw,int vh);
  void getCenterPix(char unsigned *ImPix,int vw,int vh);
  void getCenter();
  void findFreq(void);
  void playMidi(void);
  void fadeMidi(void);

  vector <float> lAudio;
  vector <float> rAudio;
  vector <float> pitchDect;
  vector <unsigned char> ImPix;
  //ofxColorImage ImBinary;
  ofxCvGrayscaleImage ImBinary;
  //------------------- for the simple sine wave synthesis
  keyParam Par;
  float mouseFrequency;
  float hPhase[NPhTot*NHarm];
  float vRegister[NHarm*NRegister];
  int NoteStat[NNoteTot];
  keyFinger Fing[NPhTot];

  ofxMidiOut midiOut;
};
#endif //TEST_APP_H
