#include "testApp.h"

keyFinger::keyFinger(){
  volume = 0.1;
  control = 0.1;
  NoteTime = 0;
  NoteQuant = 0;
  State = 0;
  NoteCounter = 0;
  midiVel = 0;
  phase = 0.;
  sPan = .5;
  phaseAdder = 0.;
  phaseAdderTarget = 0.;
  sender.setup(HOST, PORT);
}
void keyFinger::setId(int i){
  id = i;
}
void keyFinger::assignNote(keyParam Par,  cv::Point2f Cent,float area){
  OldNoteQuant = NoteQuant;
  float Pitch = 0.;
  float CentNorm = 1./((float)Par.vw);
  x = Cent.x;
  y = Cent.y;
  int vx = Par.NNote - (int)(Cent.x*CentNorm*Par.NNote);
  if(!Par.IsMirror) vx = (int)(Cent.x*CentNorm*Par.NNote);
  if(vx < 0  || vx >= Par.NNote) return;
  NoteQuant = vx;
  midiVel = (int)(Par.MidiGain*area*90.);
  midiVel = midiVel < 90 ? midiVel : 90;
  if(Par.IsQuant) Pitch = Par.vNote[vx];
  else{
    if(Par.IsMirror) Pitch = 2000.*(Par.vw-Cent.x)*CentNorm;
    else Pitch = 2000.*Cent.x*CentNorm;
  }
  phaseAdderTarget = (Pitch / (float) Par.sampleRate) * TWO_PI;
  control = Cent.y/(float)(Par.SliceU-Par.SliceD);
  volume = Par.AudioGain*area;
  volume = volume < 1. ? volume : 1.;
  while (phase > TWO_PI){
    phase -= TWO_PI;
  }
  if(Par.IsPortamento) phase = 0.;
  phaseAdder = 0.95f * phaseAdder + 0.05f * phaseAdderTarget;
  frequency = phaseAdderTarget*Par.sampleRate/TWO_PI;
}
void keyFinger::evolve(keyParam Par){
  if(State == 1) return;
  if(State == 0) return;
  NoteTime += Par.dt;
  if(State == 3){
    volume *= Par.decay(NoteTime);
    if(NoteTime >= Par.SynthTime[1]){
	NoteTime = 0.;
	State = 1;
      }}
    if(State == 2){
      volume *= Par.attack(NoteTime);
      if(NoteTime >= Par.SynthTime[0]){
	NoteTime = 0.;
	State = 3;
      }}
    if(State == -1){
      volume *= Par.release(NoteTime);
      if(NoteTime >= Par.SynthTime[3]){
	NoteTime = 0.;
	State = 0;
      }}
}
int keyFinger::findHistory(int *NewPos,int NPos,keyParam Par){
  for(int p=0;p<NPos;p++){
    if(abs(NoteQuant - NewPos[p]) < Par.PitchDist){
      return 1;
    }
  }
  return 0;
}
void keyFinger::sendOsc(){
  ofxOscMessage m;
  m.setAddress("/finger");
  m.addIntArg(id);
  m.addIntArg(NoteQuant);
  m.addFloatArg(frequency);
  m.addFloatArg(volume);
  sender.sendMessage(m);
}
string keyFinger::reportString(keyParam Par){
  string repStr = "";
  if(State == 0) return repStr;
  repStr += " f " + ofToString(frequency, 2) + " v " + ofToString(volume, 2) + " s " + ofToString(State, 0) + " ";
  return repStr;
}
