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
  int NCorrW;
  int NCorrH;
  int ColTh;
  int NoteMin;
  int NNote;
  int FrameRate;
  int bufferSize;
  int sampleRate;
  int vw;
  int vh;
  int SliceD;
  int SliceU;
  int midiChannel;
  int midiPort;
  unsigned int midiProg;
  int PitchDist;
  float timbro[16];
  float SynthTime[4];
  float SynthVol[4];
  float AudioGain;
  float MidiGain;
  float SliceFactD;
  float SliceFactU;
  float ContMinA;
  float ContMaxA;
  float dt;
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

  ofVideoGrabber video;
  ofxCv::ContourFinder contourFinder;
	
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
  void audioOut(float * input, int bufferSize, int nChannels);
  
  ofSoundStream soundStream;

  float 	pan;
  int		sampleRate;

  void envelope(int *NotePos,cv::Point2f *center,float *area,int NPhOld);
  void notEnvelope(cv::Point2f *center,float *area);
  void displayFinger();
  void clearSample(int bufferSize);
  void addWhite(int bufferSize);
  void addSine(int bufferSize);
  void addTestSine(int bufferSize);
  void addSquare(int bufferSize);
  void addHarm(int bufferSize);

  void getCenterCont(char unsigned *ImPix,int vw,int vh);
  void getCenterPix(char unsigned *ImPix,int vw,int vh);
  void getCenter();
  void findFreq(void);
  void followSignal(int buffersize);
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
