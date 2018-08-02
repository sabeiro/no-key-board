#include "testApp.h"
float vTimbro[16] = {.9,.31,.35,.68,.04,.31,.32,.41,.18,.26,.20,.20,.25,.13,.10,.08};
float NoteFreq[89] = {31.,  33., 35.,  37., 39.,  41.,  44., 46.,  49., 52.,  55., 58.,  62.,  65., 69.,  73., 78.,  82.,  87., 93.,  98., 104.,  110., 117.,  123.,  131., 139.,  147., 156.,  165.,  175., 185.,  196., 208.,  220., 233.,  247.,  262., 277.,  294., 311.,  330.,  349., 370.,  392., 415.,  440., 466.,  494.,  523., 554.,  587., 622.,  659.,  698., 740.,  784., 831.,  880., 932.,  988.,  1047., 1109.,  1175., 1245.,  1319.,  1397., 1480.,  1568., 1661.,  1760., 1865.,  1976.,  2093., 2217.,  2349., 2489.,  2637.,  2794., 2960.,  3136., 3322.,  3520., 3729.,  3951.,  4186., 4435.,  4699., 4978.};

keyParam::keyParam(){
  IsSine = true;
  IsTestSine = false;
  IsHarm = false;
  IsSquare = false;
  IsQuant = true;
  IsContour = true;
  IsAudio = false;
  IsVideo = false;
  IsMidi = false;
  IsWhite = false;
  IsEnvelope = false;
  IsFollowing = false;
  IsMirror = false;
  IsPortamento = true;
  IsInvert = false;
  NCorrW = 20;//h contour correlation
  NCorrH = 20;//v contour correlation
  ColInit = 200;//color threshold
  NoteMin = 35;//minimum note
  NNote = 24;//note range
  AudioGain = .004;
  MidiGain = 4.;
  SliceFactD = 1./4.;
  SliceFactU = 3./4.;
  ContMinA = 2.;
  ContMaxA = 20.;
  NReg = 0;
  FrameRate = 25;
  midiProg = 1;
  midiChannel = 1;
  midiPort = 1;
  PitchDist = 10;
  bufferSize = 512;
  vw = 320;//h resolution
  vh = 240;//w resolution
  sampleRate = 44100;
  SynthTime[0] = 300.;
  SynthTime[1] = 100.;
  SynthTime[2] = 300.;//ignored
  SynthTime[3] = 200.;
  SynthVol[0] = 1.;
  SynthVol[1] = 1.;
  SynthVol[2] = .8;
  SynthVol[3] = .0;
  for(int t=0;t<16;t++){
    timbro[t] = vTimbro[t];
  }
  calcDependencies();
  parameters.setName("spot shape");
  parameters.add(SliceD.set("up",40,1,120));
  parameters.add(SliceU.set("down",140,120,240));  
  parameters.add(NoteMin.set("note min",12,0,24));
  parameters.add(NNote.set("note range",12,12,36));  
  parameters.add(check.set("check",false));
  parameters.add(ContMaxA.set("r max",10.,10.,60.));  
  parameters.add(ContMinA.set("r min",2.,1.,20.));
  parameters.add(colTh.set("color",ofColor(127),ofColor(0,0),ofColor(255)));
}
void keyParam::calcDependencies(){
  dt = 1./(float)FrameRate*1000.;
  SliceD = (int)(vh*SliceFactD);
  SliceU = (int)(vh*SliceFactU);
  SliceU = SliceU > SliceD ? SliceU : SliceD;
  SliceD = SliceU < SliceD ? SliceU : SliceD;
  for(int n=0;n<NNote;n++)
    vNote[n] = NoteFreq[n+NoteMin];
  cout << "dt:  " << dt << endl;
}
void keyParam::readConf(char *FName){
  FILE *File2Open;
  float buff[16];
  if( (File2Open = fopen(FName,"r"))==0){
    printf("The file %s does not exist\n",FName);
    return ;
  }
  char cLine[256];
  for(int k=0;!(fgets(cLine,256,File2Open)==NULL);k++){
    if(cLine[0] == '#') continue;
    if(1 == sscanf(cLine,"ColThreshold %f",buff) )
      ColInit = (int)*buff;
    if(1 == sscanf(cLine,"midiPort %f",buff) )
	midiPort = (int)*buff;
    if(1 == sscanf(cLine,"midiProg %f",buff) )
      midiProg = (int)*buff;
    if(1 == sscanf(cLine,"NoteMin %f",buff) )
      NoteMin = (int)*buff;
    if(1 == sscanf(cLine,"NNote %f",buff) )
	NNote = (int)*buff;
    if(1 == sscanf(cLine,"FrameRate %f",buff) )
	FrameRate = (int)*buff;
      if(2 == sscanf(cLine,"CorrLength %f %f",buff,buff+1) ){
	NCorrW = (int)*buff;
	NCorrH = (int)buff[1];
      }
      if(1 == sscanf(cLine,"AudioGain %f",buff) )
	AudioGain = *buff;
      if(1 == sscanf(cLine,"MidiGain %f",buff) )
	MidiGain = *buff;
      if(1 == sscanf(cLine,"PitchDist %f",buff) )
	PitchDist = (int)*buff;
      if(2 == sscanf(cLine,"SliceFact %f %f",buff,buff+1) ){
	SliceFactD = *buff;
	SliceFactU = buff[1];
      }
      if(2 == sscanf(cLine,"ContArea %f %f",buff,buff+1) ){
	ContMinA = *buff;
	ContMaxA = buff[1];
      }
      if(3 == sscanf(cLine,"SynthTime %f %f %f %f",buff,buff+1,buff+2,buff+3) ){
	for(int i=0;i<4;i++) SynthTime[i] = buff[i];
      }
      if(4 == sscanf(cLine,"SynthVol %f %f %f %f",buff,buff+1,buff+2,buff+3) ){
	for(int i=0;i<4;i++) SynthVol[i] = buff[i];
      }
      for(int r=0;r<NRegister;r++){
	char RegName[256];
	sprintf(RegName,"Timbro%d %%f %%f %%f %%f %%f %%f %%f %%f %%f %%f %%f %%f %%f %%f %%f %%f",r);
	if(16 == sscanf(cLine,RegName,buff,buff+1,buff+2,buff+3,buff+4,buff+5,buff+6,buff+7,buff+8,buff+9,buff+10,buff+11,buff+12,buff+13,buff+14,buff+15))
	  for(int i=0;i<NHarm;i++) vTimbro[r*NHarm+i] = buff[i];
      }
  }
  calcDependencies();
  fclose(File2Open);
}
void keyParam::writeConf(char *FName){
  ofstream F2Write;
  F2Write.open(FName);
  F2Write << "ColThreshold " << ColTh << "\n";
  F2Write << "midiPort " << midiPort << "\n";
  F2Write << "midiProg " << midiProg << "\n";
  F2Write << "NoteMin " << NoteMin << "\n";
  F2Write << "NNote " << NNote << "\n";
  F2Write << "CorrLength " << NCorrW << " " << NCorrH << "\n";
  F2Write << "AudioGain " << AudioGain << "\n";
  F2Write << "MidiGain " << MidiGain << "\n";
  F2Write << "PitchDist " << PitchDist << "\n";
  F2Write << "SliceFact " << SliceFactD << " " << SliceFactU << "\n";
  F2Write << "ContArea " << ContMinA << " " <<ContMaxA << "\n";
  F2Write << "SynthTime ";
  for(int i=0;i<4;i++) F2Write << SynthTime[i] << " ";
  F2Write << "\n";
  F2Write << "SynthVol ";
  for(int i=0;i<4;i++) F2Write << SynthVol[i] << " ";
  F2Write << "\n";
  for(int r=0;r<NRegister;r++){
    F2Write << "#Timbro" << r << " ";
    for(int i=0;i<NHarm;i++){
      F2Write << vTimbro[r*NHarm+i] << " ";
    }
    F2Write << "\n";
  }
  F2Write.close();
}
float keyParam::attack(float time){
  return time/SynthTime[0]*SynthVol[0];
}
float keyParam::decay(float time){
  return time/SynthTime[1]*(SynthVol[1] + SynthVol[2]) - SynthVol[1];
}
float keyParam::release(float time){
  return time/SynthTime[3]*(SynthVol[2] + SynthVol[3]) - SynthVol[2];
}
string keyParam::reportString(){
  string repStr = " ";
  repStr += "[s] "; if(IsSine) repStr += "sine ";
  repStr += "[S] "; if(IsSquare) repStr += "square ";
  repStr += "[h] "; if(IsHarm) repStr += "harmonics ";
  repStr += "[q] "; if(IsQuant) repStr += "quant ";
  repStr += "[c] "; if(IsContour) repStr += "contour ";
  repStr += "[p] "; if(IsAudio) repStr += "audio ";
  repStr += "[o] "; if(IsMidi) repStr += "osc ";
  repStr += "[m] "; if(IsMidi) repStr += "midi ";
  repStr += "[M] "; if(IsMirror) repStr += "mirror ";
  repStr += "[P] "; if(IsPortamento) repStr += "portamento ";
  repStr += "[w] "; if(IsWhite) repStr += "white ";
  repStr += "[e] "; if(IsEnvelope) repStr += "envelope ";
  repStr += "\n";
  repStr += "ColTh " + ofToString(ColTh, 2);
  repStr += " NCorr " + ofToString(NCorrW, 0) + "  " + ofToString(NCorrH, 0);
  repStr += " SliceFact " + ofToString(SliceFactD,2) + "  " + ofToString(SliceFactU,2);
  repStr += " ContArea " + ofToString(ContMinA,0) + "  " + ofToString(ContMaxA,0);
  repStr += " Gain " + ofToString(AudioGain,4) + " " + ofToString(MidiGain,2);
  repStr += " Register " + ofToString(NReg,0);
  repStr += " midiProg " + ofToString(midiProg,0);
  repStr += " frameRate " + ofToString(FrameRate,0);
  repStr += "\n";
  return repStr;
}
