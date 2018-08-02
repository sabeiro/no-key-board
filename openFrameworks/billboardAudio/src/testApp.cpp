#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {

  ofBackground(0, 0, 0);
	
  cameraRotation.set(0);
  zoom = -500;
  zoomTarget = 000;
	
  billboards.getVertices().resize(NUM_BILLBOARDS);
  billboards.getColors().resize(NUM_BILLBOARDS);
  billboards.getNormals().resize(NUM_BILLBOARDS,ofVec3f(0));
	
  // ------------------------- billboard particles
  for (int i=0; i<NUM_BILLBOARDS; i++) {		
    billboardVels[i].set(ofRandomf(), -1.0, ofRandomf());
    billboards.getVertices()[i].set(ofRandom(-500, 500), 
				    ofRandom(-500, 500), 
				    ofRandom(-50, 50));
		
    billboards.getColors()[i].set(ofColor::fromHsb(ofRandom(96, 160), 255, 255));
    billboardSizeTarget[i] = ofRandom(4, 64);
  }

  billboards.setUsage( GL_DYNAMIC_DRAW );
  billboards.setMode(OF_PRIMITIVE_POINTS);
  // billboardVbo.setVertexData(billboardVerts, NUM_BILLBOARDS, GL_DYNAMIC_DRAW);
  // billboardVbo.setColorData(billboardColor, NUM_BILLBOARDS, GL_DYNAMIC_DRAW);
	
  // load the bilboard shader 
  // this is used to change the
  // size of the particle
  billboardShader.load("Billboard");
	
  // we need to disable ARB textures in order to use normalized texcoords
  ofDisableArbTex();
  texture.loadImage("dot.png");
  ofEnableAlphaBlending();

  //  my_sound.loadSound("Zoe.wav");
  IfSong = 0;
  // the fft needs to be smoothed out, so we create an array of floats
  // for that purpose:
  fftSmoothed = new float[8192];
  for (int i = 0; i < 8192; i++){
    fftSmoothed[i] = 0;
  }
  nBandsToGet = 8;
  SongAmp = 500.;

  soundStream.listDevices();
  bufferSize = 256;
  soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
  IfStream = 0;
  left.assign(bufferSize, 0.0);
  StreamAmp = 30.;
  soundStream.stop();


}

//--------------------------------------------------------------
void testApp::update() {
  float t = (ofGetElapsedTimef()) * 0.9f;
  float div = 1./250.0;

  // update the sound playing system:
  ofSoundUpdate();	

  float * val = ofSoundGetSpectrum(nBandsToGet);// request 128 values for fft
  for (int i = 0;i < nBandsToGet; i++){
    // let the smoothed calue sink to zero:
    fftSmoothed[i] *= 0.96f;
    // take the max, either the smoothed or the incoming:
    if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];
  }
  float turn = fftSmoothed[0];
  float grav = 1.f;
  
  for (int i=0; i<NUM_BILLBOARDS; i++){
    // noise 
    float xturn = ofSignedNoise(t, billboards.getVertex(i).y*div, billboards.getVertex(i).z*div);
    float yturn = ofSignedNoise(billboards.getVertex(i).x*div, t, billboards.getVertex(i).z*div);
    float zturn = ofSignedNoise(billboards.getVertex(i).x*div, billboards.getVertex(i).y*div, t);
    ofVec3f vec(xturn,yturn,zturn);
    vec *= 1 * ofGetLastFrameTime();
    vec.z += grav;
    if(IfSong){
      int vx = (int)((billboards.getVertices()[i].x+500)*0.001*nBandsToGet);
      if(vx >= 0 && vx < nBandsToGet){
	turn = fftSmoothed[vx];//*(billboards.getVertices()[i].x+500)*0.001;
	//vec.z -= SongAmp*grav*fftSmoothed[vx];
	billboards.getVertices()[i].z = StreamAmp*grav*fftSmoothed[vx];
      }
    }
    if(IfStream){
      int vy = (int)((billboards.getVertices()[i].y+500)*0.001*bufferSize);
      vy = (int)(vy/(float)bufferSize*16);
      if(vy >= 0 && vy < bufferSize){
	vec.z -= StreamAmp*grav*left[vy];
	turn = left[vy];
      }
      //{ turn += 10.f*scaledVol;}
    }
    if(billboards.getVertices()[i].z > 200)  vec.z = -fabs(vec.z);
    if(billboards.getVertices()[i].z < -100) vec.z = fabs(vec.z);
    billboardVels[i] += vec;
    billboards.getVertices()[i] += billboardVels[i];
    if(billboards.getVertices()[i].x > 500) billboards.getVertices()[i].x -= 1000;
    if(billboards.getVertices()[i].x < -500) billboards.getVertices()[i].x += 1000;
    if(billboards.getVertices()[i].y > 500) billboards.getVertices()[i].y -= 1000;
    if(billboards.getVertices()[i].y < -500) billboards.getVertices()[i].y += 1000;
    // if(billboards.getVertices()[i].z > 100) billboards.getVertices()[i].z -= 200;
    // if(billboards.getVertices()[i].z < -100) billboards.getVertices()[i].z += 200;
    billboardSizeTarget[i] = turn*ofRandom(4, 64);
    billboardVels[i] *= 0.94f; 
    billboards.setNormal(i,ofVec3f(12 + billboardSizeTarget[i] * ofNoise(t+i),0,0));
  }
    
  // // move the camera around
  // float mx = (float)mouseY/(float)ofGetWidth();
  // float my = (float)mouseX/(float)ofGetHeight();
  // ofVec3f des(mx * 360.0, my * 360.0, 0);
  // cameraRotation += (des-cameraRotation) * 0.006;
  zoom += (zoomTarget - zoom) * 0.03;

}

//--------------------------------------------------------------
void testApp::draw() {
  //ofBackgroundGradient(ofColor(255), ofColor(230, 240, 255));
  ofBackgroundGradient(ofColor(0), ofColor(0, 0, 0));
	
  string info = ofToString(ofGetFrameRate(), 2)+"\n";
  info += "Particle Count: "+ofToString(NUM_BILLBOARDS) + "\n";
  info += "stream: " + ofToString(IfStream) + " song: " + ofToString(IfSong) + "\n";
  info += "AmpSpe: " + ofToString(SongAmp) + " AmpSound: " + ofToString(StreamAmp);
  ofDrawBitmapStringHighlight(info, 30, 30);

  ofEnableAlphaBlending();
  ofSetColor(0,0,0,100);
  ofRect(100,ofGetHeight()-300,5*128,200);
  // draw the fft resutls:
  ofSetColor(255,0,255,255);
  float width = (float)(5*128) / nBandsToGet;
  for (int i = 0;i < nBandsToGet; i++){
    // (we use negative height here, because we want to flip them
    // because the top corner is 0,0)
    ofRect(100+i*width,ofGetHeight()-100,width,-(fftSmoothed[i] * 200));
  }
  ofDisableAlphaBlending();

  // draw the left channel:
  ofPushStyle();
  ofPushMatrix();
  ofTranslate(32, 170, 0);  
  ofEnableAlphaBlending();
  ofSetColor(0,0,0,100);
  ofRect(100,ofGetHeight()-300,5*128,200);
  ofSetColor(245, 58, 135,100);
  ofSetLineWidth(3);
  ofBeginShape();
  for (int i = 0; i < left.size(); i++){
    ofVertex(i*2, 100 -left[i]*180.0f);
  }
  ofDisableAlphaBlending();
  ofEndShape(false);
  ofPopMatrix();
  ofPopStyle();


  ofSetColor(255);
	
  ofPushMatrix();
  //ofTranslate(ofGetWidth()/2, ofGetHeight()/2, zoom);
  ofTranslate(zoom,zoom,0);
  cam.begin();

  // ofRotate(cameraRotation.x, 1, 0, 0);
  // ofRotate(cameraRotation.y, 0, 1, 0);
  // ofRotate(cameraRotation.y, 0, 0, 1);
	
  // bind the shader so that wee can change the
  // size of the points via the vert shader
  ofPushStyle();
  ofEnableAlphaBlending();
  billboardShader.begin();
  ofEnablePointSprites();
  texture.getTextureReference().bind();
  billboards.draw();
  texture.getTextureReference().unbind();
  ofDisablePointSprites();	
  billboardShader.end();
  ofPopStyle();
  
  cam.end();
  ofPopMatrix();//camera

  // ofEnableAlphaBlending();
  // ofNoFill();
  // ofSetColor(0,0,0,40);
  // ofBeginShape();
  // for (int i = 0; i < left.size(); i++){
  //   ofVertex(i*2, 100 -left[i]*180.0f);
  // }
  // ofEndShape(true);
    
  // ofSetColor(0,0,0,80);
  // for (int i = 0; i < nCurveVertices; i++){
  //   if (curveVertices[i].bOver == true) ofFill();
  //   else ofNoFill();
  //   ofCircle(curveVertices[i].x, curveVertices[i].y,4);
  // }
  // ofDisableAlphaBlending();

}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
  if(key == 'f') ofToggleFullscreen();
  if(key == OF_KEY_UP) zoomTarget +=10;
  if(key == OF_KEY_DOWN) zoomTarget -=10;
}


//--------------------------------------------------------------
void testApp::keyReleased(int key){
  if(key == 'p'){ 
    if(IfSong){my_sound.setPaused(true); IfSong = 0;}
    else { my_sound.play(); IfSong = 1;}
  }
  if(key == 's'){ 
    if(!IfStream){ soundStream.start(); IfStream = 1;}
    else { soundStream.stop(); IfStream = 0;}
  }
  if(key == 'g') SongAmp += 5.;
  if(key == 'G') SongAmp -= 5.;
  if(key == 'h') StreamAmp += 5.;
  if(key == 'H') StreamAmp -= 5.;
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){		
  float curVol = 0.0;
  // samples are "interleaved"
  int numCounted = 0;
  //lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
  for (int i = 0; i < bufferSize; i++){
    left[i] = input[i*2]*0.5;
    float right = input[i*2+1]*0.5;
    curVol += left[i] * left[i];
    curVol += right * right;
    numCounted+=2;
  }
  //this is how we get the mean of rms :) 
  curVol /= (float)numCounted;
  // this is how we get the root of rms :) 
  curVol = sqrt( curVol );
	
  smoothedVol *= 0.93;
  smoothedVol += 0.07 * curVol;
	
  bufferCounter++;
  scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);	
}
