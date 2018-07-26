#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
  // ofEnableSmoothing();
  ofSetFrameRate(Par.FrameRate);
  ofSetVerticalSync(true);
  Par.readConf("data/pianoConf.txt");
  cout << Par.reportString();
  //midi
  midiOut.listPorts(); // via instance
  //midiOut.openPort("IAC Driver Pure Data In");	// by name
  //midiOut.openVirtualPort("ofxMidiOut");		// open a virtual port
  //camera
  video.listDevices();
  video.setDeviceID(2);
  video.initGrabber(Par.vw,Par.vh);
  contourFinder.setMinAreaRadius(Par.ContMinA);
  contourFinder.setMaxAreaRadius(Par.ContMaxA);
  //audio
  lAudio.assign(Par.bufferSize, 0.0);
  pitchDect.assign(Par.bufferSize, 0.0);
  rAudio.assign(Par.bufferSize, 0.0);
  ImPix.assign(Par.vw*Par.vh,0);
  ImBinary.allocate(Par.vw,Par.SliceU-Par.SliceD);
  //soundStream.listDevices();
  //if you want to set the device id to be different than the default
  //soundStream.setDeviceID(1); 	//note some devices are input only and some are output only 
  soundStream.setup(this, 2, 0, Par.sampleRate, Par.bufferSize, 4);
  soundStream.stop();
  for(int p=0;p<NPhTot;p++){Fing[p].setId(p);}
}

//--------------------------------------------------------------
void testApp::update(){
  video.update();
}
void testApp::envelope(int *NotePos,cv::Point2f *center,float *area,int NPhOld){
  if(NPhase == NPhOld){
    notEnvelope(center,area);
    return;
  }
  int active = NPhase > NPhOld ? NPhase : NPhOld;
  for(int p=0;p<active;p++){
    int IsPresent = Fing[p].findHistory(NotePos,NPhase,Par);
    if(!IsPresent){//not present in history (not within +- PitchDist)
      if(NPhase > NPhOld){//new note
	for(int p1=0;p1<active;p1++){
	  if(Fing[p1].State == 0){//fill the first available note
	    Fing[p1].State = 2;
	    Fing[p1].assignNote(Par,center[p],area[p]);
	    break;
	  }}}
      else if(NPhase < NPhOld){//fading note
	Fing[p].State = -1;
      }}
    else{//update existing
      Fing[p].assignNote(Par,center[p],area[p]);
    }}
}
void testApp::notEnvelope(cv::Point2f *center,float *area){
  for(int p=0;p<NPhase;p++){
    Fing[p].State = 1;
    Fing[p].assignNote(Par,center[p],area[p]);
  }
  for(int p=NPhase;p<NPhTot;p++){
    Fing[p].State = 0;
  }
}
void testApp::getCenter(){
  if (!video.isFrameNew()) return;
  contourFinder.setThreshold(Par.ColTh);
  unsigned char *pixels = video.getPixels();
  //ImPix.assign(Par.vw*Par.vh,0);
  ImBinary.set(1.0);
  for(int w=0;w<Par.vw;w++){
    for(int h=Par.SliceD;h<Par.SliceU;h++){
      if(Par.IsInvert){
	if(pixels[(h*Par.vw+w)*3  ] < Par.ColTh){
	  int h1 = h - Par.SliceD;
	  ImBinary.getPixels()[h1*Par.vw+w] = 255;
	  //ImPix[h1*Par.vw+w] = 255;
	}
      }
      else{
	if(pixels[(h*Par.vw+w)*3  ] > Par.ColTh){
	  int h1 = h - Par.SliceD;
	  ImBinary.getPixels()[h1*Par.vw+w] = 255;
	  //ImPix[h1*Par.vw+w] = 255;
	}
      }
    }
  }
  //ImBinary.setFromPixels(ImPix,Par.vw,Par.SliceU-Par.SliceD);
  //contourFinder.findContours(video);
  contourFinder.findContours(ImBinary);
  int NPhOld = NPhase;
  NPhase = contourFinder.size();
  int NotePos[NPhTot];
  float area[NPhTot];
  cv::Point2f center[NPhTot];
  for(int p=0;p<NPhase;p++){
    //int x = contourFinder.getCenter(p).x;
    //if(y < Par.SliceD || y >= Par.SliceU) continue;
    center[p] = contourFinder.getCenter(p);
    area[p] = contourFinder.getContourArea(p);
    NotePos[p] = (int)center[p].x;
    // center[NPhase] = contourFinder.getCenter(p);
    // area[NPhase] = contourFinder.getContourArea(p);
    // NotePos[NPhase] = (int)center[NPhase].x;
    // NPhase++;
    // if(NPhase >= NPhTot) break;
  }
  if(Par.IsEnvelope){
    envelope(NotePos,center,area,NPhOld);
  }
  else{
    notEnvelope(center,area);
  }
}
void testApp::playMidi(){
  for(int p=0;p<NPhTot;p++){
    if(Fing[p].State == 0) return;
    if(Fing[p].State == -1){Fing[p].State = 0; return;}
    int vx = Fing[p].NoteQuant;
    int note = vx + Par.NoteMin;
    if(NoteStat[vx] == 0){
      midiOut.sendNoteOn(Par.midiChannel, note, Fing[p].midiVel);
      NoteStat[vx] = 1;
    }
    if(!Par.IsQuant){
      float CentNorm = 1./((float)Par.vw);
      float BendNorm = Par.NNote/(float)Par.vw;
      float diff = Fing[p].x - vx/Par.NNote*Par.vw;
      int PitchDiff = (int)(diff*BendNorm*64.);
      PitchDiff = PitchDiff > 64 ? 64 : PitchDiff;
      PitchDiff += 64;
      //printf("%f %f %f %d %d\n",Fing[p].x ,Fing[p].NoteQuant/Par.NNote*Par.vw,diff,PitchDiff,Fing[p].NoteQuant);
      midiOut.sendPitchBend(p,PitchDiff);
    }
  }
}
void testApp::fadeMidi(void){
  for(int n=0;n<Par.NNote;n++){
    if(NoteStat[n] == 1){
      bool IsContinue = 1;
      for(int p=0;p<NPhTot;p++){
	if(Fing[p].NoteQuant == n){
	  IsContinue = 0;
	  break;
	}
      }
      if(IsContinue){
	NoteStat[n] = 0;
	int note = n + Par.NoteMin;
	midiOut.sendNoteOff(Par.midiChannel, note,  0);
  	//midiOut.sendNoteOn(Par.midiChannel, note,  0);
      }}}
  if(NPhase=0){
    for(int n=0;n<Par.NNote;n++){
      int note = n + Par.NoteMin;
      midiOut.sendNoteOff(Par.midiChannel, note,  0);
      midiOut.sendNoteOn(Par.midiChannel, note,  0);
    }}
}
void testApp::findFreq(){
  getCenter();
  if(Par.IsAudio){for(int p=0;p<NPhTot;p++) Fing[p].evolve(Par);}
  if(Par.IsFollowing){followSignal(Par.bufferSize);}
  if(Par.IsOsc){for(int p=0;p<NPhTot;p++){if(Fing[p].State!=0)Fing[p].sendOsc();}}
  if(Par.IsMidi){
    playMidi();
    fadeMidi();
  }
}
//--------------------------------------------------------------
void testApp::draw(){
  ofBackgroundGradient(ofColor(50), ofColor(0));
  if(Par.IsVideo){
    findFreq();
    video.draw(0, 0);
    int width = ofGetWidth();
    ImBinary.draw(width/2-Par.vw/2,0);
    contourFinder.draw();
  }
  stringstream text;
  string reportString = Par.reportString();
  text << "Port " << midiOut.getPort() << " \"" << midiOut.getName() << "\"" << "is virtual?: " << midiOut.isVirtual() << endl;
  reportString += text.str();
  for(int p=0;p<NPhTot;p++){
    reportString += Fing[p].reportString(Par);
  }
  ofDrawBitmapString(reportString, 32, 579);
  ofNoFill();
  if(Par.IsAudio){
    // draw the left channel:
  ofPushStyle();
  ofPushMatrix();
  ofTranslate(32, 150, 0);
  ofSetColor(225);
  ofDrawBitmapString("Left Channel", 4, 18);
  ofSetLineWidth(1);	
  ofRect(0, 0, 900, 200);
  ofSetLineWidth(3);
  ofSetColor(245, 58, 135);
  ofBeginShape();
  for (unsigned int i = 0; i < lAudio.size(); i++){
    float x =  ofMap(i, 0, lAudio.size(), 0, 900, true);
    ofVertex(x, 100 -lAudio[i]*180.0f);
  }
  ofEndShape(false);
  ofSetColor(245, 225, 135);
  if(Par.IsFollowing){
    ofBeginShape();
    for (unsigned int i = 0; i < pitchDect.size(); i++){
      float x =  ofMap(i, 0, pitchDect.size(), 0, 900, true);
      ofVertex(x, 100 - pitchDect[i]*180.0f);
    }
  }
  ofEndShape(false);
  ofPopMatrix();
  ofPopStyle();
  // draw the right channel:
  ofPushStyle();
  ofPushMatrix();
  ofTranslate(32, 350, 0);
  ofSetColor(225);
  ofDrawBitmapString("Right Channel", 4, 18);
  ofSetLineWidth(1);	
  ofRect(0, 0, 900, 200);
  ofSetLineWidth(3);
  ofSetColor(245, 58, 135);
  ofBeginShape();
  for (unsigned int i = 0; i < rAudio.size(); i++){
    float x =  ofMap(i, 0, rAudio.size(), 0, 900, true);
    ofVertex(x, 100 -rAudio[i]*180.0f);
  }
  ofEndShape(false);
  ofSetColor(245, 58, 135);
  ofBeginShape();
  for (unsigned int i = 0; i < rAudio.size(); i++){
    float x =  ofMap(i, 0, rAudio.size(), 0, 900, true);
    ofVertex(x, 100 -rAudio[i]*180.0f);
  }
  ofEndShape(false);
  ofPopMatrix();
  ofPopStyle();
  // draw the envelope:
  ofPushStyle();
  ofPushMatrix();
  ofTranslate(950, 150, 0);
  ofSetColor(225);
  string label = "Envelope " + ofToString(Par.SynthTime[0]) + " " + ofToString(Par.SynthTime[1]) + " " + ofToString(Par.SynthTime[3]) + " ms\n" ;
  ofDrawBitmapString(label, 4, 18);
  ofSetLineWidth(1); 
  ofRect(0, 0, 200, 200);
  ofSetColor(245, 58, 135);
  ofSetLineWidth(3);
  ofBeginShape();
  float L = 200./(Par.SynthTime[0] + Par.SynthTime[1] + 2.*Par.SynthTime[2] + Par.SynthTime[3]);
  ofVertex(0,200.);
  float xEnv = Par.SynthTime[0]*L;
  ofVertex(xEnv,200.-Par.SynthVol[0]*200.);
  xEnv += Par.SynthTime[1]*L;
  ofVertex(xEnv,200.-Par.SynthVol[1]*200.);
  xEnv += Par.SynthTime[2]*L;
  ofVertex(xEnv,200.-Par.SynthVol[2]*200.);
  xEnv += Par.SynthTime[2]*L;
  ofVertex(xEnv,200.-Par.SynthVol[2]*200.);
  xEnv += Par.SynthTime[3]*L;
  ofVertex(xEnv,200.-Par.SynthVol[3]*200.);
  ofEndShape(false);
  ofPopMatrix();
  ofPopStyle();
  }//Audio
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
  if (key == '-' || key == '_' ){
  } else if (key == '+' || key == '=' ){
  }
  if( key == 'c' ){
    Par.IsContour = !Par.IsContour;
  }
  if( key == 'e' ){
    Par.IsEnvelope = !Par.IsEnvelope;
  }
  if( key == 'f' ){
    Par.IsFollowing = !Par.IsFollowing;
  }
  if( key == 'i' ){
    Par.IsInvert = !Par.IsInvert;
  }
  if( key == 'h' ){
    Par.IsHarm = !Par.IsHarm;
    if(Par.IsHarm) Par.AudioGain /= 4.;
    else Par.AudioGain *= 4.;
  }
  if( key == 'o' ){
    Par.IsOsc = !Par.IsOsc;
  }
  if( key == 'm' ){
    Par.IsMidi = !Par.IsMidi;
    if(Par.IsMidi){
      midiOut.openPort(Par.midiPort);// by number
    }
    else{
      for(int n=0;n<Par.NNote;n++){
	int note = n + Par.NoteMin;
	midiOut.sendNoteOff(Par.midiChannel, note,  0);
  	//midiOut.sendNoteOn(Par.midiChannel, note,  0);
      }    
      midiOut.closePort();
    }
  }
  if( key == 'M' ){
    Par.IsMirror = !Par.IsMirror;
  }
  if( key == 'p' ){
    Par.IsAudio = !Par.IsAudio;
    if(Par.IsAudio){
      soundStream.start();
    }
    else{
      soundStream.stop();
    }
  }
  if( key == 'P' ){
    Par.IsPortamento = !Par.IsPortamento;
  }
  if( key == 'q' ){
    Par.IsQuant = !Par.IsQuant;
  }
  if( key == 'r'){
    Par.readConf("data/pianoConf.txt");
  } 
  if( key == 'R'){
    Par.NReg++;
    if(Par.NReg >= NRegister) Par.NReg = 0;
  }
  if( key == 's' ){
    Par.IsSine = !Par.IsSine;
  }
  if( key == 'S' ){
    Par.IsSquare = !Par.IsSquare;
  }
  if( key == 't' ){
    Par.IsTestSine = !Par.IsTestSine;
  }
  if( key == 'v' ){
    Par.IsVideo = !Par.IsVideo;
  }
  if( key == 'w' ){
    Par.IsWhite = !Par.IsWhite;
  }
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){
  switch(key) {
  case OF_KEY_UP:
    Par.midiProg = (int) ofClamp(Par.midiProg+1, 0, 127);
    midiOut.sendProgramChange(Par.midiChannel, Par.midiProg);
    break;
  case OF_KEY_DOWN:
    Par.midiProg = (int) ofClamp(Par.midiProg-1, 0, 127);
    midiOut << ProgramChange(Par.midiChannel, Par.midiProg); // stream interface
    break;
  }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
  // int width = ofGetWidth();
  // float height = (float)ofGetHeight();
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::clearSample(int bufferSize){
  for (int i = 0; i < bufferSize; i++){
    lAudio[i] =  0.;
    rAudio[i] = 0.;
  }  
  for(int p=0;p<NPhase;p++){
    Fing[p].sPan = (float)(p+1)/(float)NPhase;
  }
}
void testApp::addWhite(int bufferSize){
  for (int i = 0; i < bufferSize; i++){
    lAudio[i] += .1*ofRandom(0, 1);
    rAudio[i] += .1*ofRandom(0, 1);
  }
}
void testApp::addTestSine(int bufferSize){
  float pitch = (220. / (float) Par.sampleRate) * TWO_PI;
  for (int i = 0; i < bufferSize; i++){
    float sample = 0.;
    Fing[0].phase += pitch;
    sample = sin(Fing[0].phase);
    lAudio[i] += .5*sample;
    rAudio[i] += .5*sample;
  }
}
void testApp::addSine(int bufferSize){
  for (int i = 0; i < bufferSize; i++){
    float sample = 0.;
    for(int p=0;p<NPhTot;p++){
      if(Fing[p].State == 0) continue;
      Fing[p].phase += Fing[p].phaseAdder;
      sample = Fing[p].volume*sin(Fing[p].phase);
      lAudio[i] += sample*Fing[p].sPan;
      rAudio[i] += sample*(1.-Fing[p].sPan);
    }
  }  
}
void testApp::addHarm(int bufferSize){
  for(int h=0;h<NHarm;h++){
    for(int p=0;p<NPhTot;p++){
      while (hPhase[h*NPhTot+p] > TWO_PI){
	hPhase[h*NPhTot+p] -= TWO_PI;
      }
    }
    for (int i = 0; i < bufferSize; i++){
      float sample = 0.;
      for(int p=0;p<NPhTot;p++){
	if(Fing[p].State == 0) continue;
	hPhase[h*NPhTot+ p] += (h+2)*Fing[p].phaseAdder;
	sample = Par.timbro[h]*Fing[p].volume*sin(hPhase[h*NPhTot+ p]);
	lAudio[i] += sample*Fing[p].sPan;
	rAudio[i] += sample*(1.-Fing[p].sPan);
      }}}
}
void testApp::addSquare(int bufferSize){
  for (int i = 0; i < bufferSize; i++){
    float sample = 0.;
    for(int p=0;p<NPhTot;p++){
      if(Fing[p].State == 0) continue;
      Fing[p].phase += Fing[p].phaseAdder;
      if(sin(Fing[p].phase) > 0.) sample = Fing[p].volume;
      else sample = -Fing[p].volume;
      lAudio[i] += sample*Fing[p].sPan;
      rAudio[i] += sample*(1.-Fing[p].sPan);
    }}
}
//--------------------------------------------------------------
void testApp::audioOut(float * output, int bufferSize, int nChannels){
  // sin (n) seems to have trouble when n is very large, so we
  // keep phase in the range of 0-TWO_PI like this:
  clearSample(bufferSize);
  if(Par.IsWhite) addWhite(bufferSize);
  if(Par.IsSine) addSine(bufferSize);
  if(Par.IsTestSine) addTestSine(bufferSize);
  if(Par.IsHarm) addHarm(bufferSize);
  if(Par.IsSquare) addSquare(bufferSize);
  for (int i = 0; i < bufferSize; i++){
    output[i*nChannels       ] = lAudio[i];
    output[i*nChannels + 1] = rAudio[i];
  }
}
void testApp::followSignal(int buffersize){
  bool clipping = 0;
  unsigned int time = 0;//keeps time and sends vales to store in timer[] occasionally
  int timer[10];//sstorage for timing of events
  int slope[10];//storage fro slope of events
  unsigned int totalTimer = 0;//used to calculate period
  unsigned int period = 0;//storage for period of wave
  int index = 0;//current storage index
  float frequency = 0.;//storage for frequency calculations
  float maxSlope = 0;//used to calculate max slope as trigger point
  float newSlope = 0.;//storage for incoming slope data
  int noMatch = 0;//counts how many non-matches you've received to reset variables if it's been too long
  float slopeTol = 3;//slope tolerance- adjust this if you need
  float slopeMax = 0.;
  int timerTol = 10;//timer tolerance- adjust this if you need
  float quant = 2.;
  for(int i=0;i<buffersize-1;i++){
    pitchDect[i] = (int)(lAudio[i]*quant);
    pitchDect[i+1] = (int)(lAudio[i+1]*quant);
    if (pitchDect[i] < 127. && pitchDect[i+1] >= 127.){//if increasing and crossing midpoint
      newSlope = pitchDect[i+1] - pitchDect[i];//calculate slope
      if (abs(newSlope-maxSlope)<slopeTol){//if slopes are ==
	//record new data and reset time
	slope[index] = newSlope;
	timer[index] = time;
	time = 0;
	if (index == 0){//new max slope just reset
	  noMatch = 0;
	  index++;//increment index
	}
	else if (abs(timer[0]-timer[index])<timerTol && abs(slope[0]-newSlope)<slopeTol){//if timer duration and slopes match
	  //sum timer values
	  totalTimer = 0;
	  for (int i=0;i<index;i++){
	    totalTimer+=timer[i];
	  }
	  period = totalTimer;//set period
	  //reset new zero index values to compare with
	  timer[0] = timer[index];
	  slope[0] = slope[index];
	  index = 1;//set index to 1
	  noMatch = 0;
	}
	else{//crossing midpoint but not match
	  index++;//increment index
	  if (index > 9){
	    index = 0;
	    slopeMax = 0.;
	    noMatch = 0;
	  }
	}
      }
      else if (newSlope>maxSlope){//if new slope is much larger than max slope
	maxSlope = newSlope;
	time = 0;//reset clock
	noMatch = 0;
	index = 0;//reset index
      }
      else{//slope not steep enough
	noMatch++;//increment no match counter
	if (noMatch>9){
	    index = 0;
	    slopeMax = 0.;
	    noMatch = 0;
	}
      }
      pitchDect[i] *= quant;;
    }    
    time++;//increment timer at rate of 38.5kHz
  }
  frequency = 38462/float(period);//calculate frequency timer rate/period
}
//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
void testApp::exit() {	
  // clean up
  midiOut.closePort();
}
