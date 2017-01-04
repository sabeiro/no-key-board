#include "MSAPhysics3D.h"
#include "testApp.h"

#define	SPRING_MIN_STRENGTH		0.005
#define SPRING_MAX_STRENGTH		0.1

#define	SPRING_MIN_WIDTH		1
#define SPRING_MAX_WIDTH		3

#define NODE_MIN_RADIUS			5
#define NODE_MAX_RADIUS			15

#define MIN_MASS				1
#define MAX_MASS				3

#define MIN_BOUNCE				0.2
#define MAX_BOUNCE				0.9

#define SQR(a) ((a)*(a))

#define	FIX_PROBABILITY			10		// % probability of a particle being fixed on creation

#define FORCE_AMOUNT			10

#define EDGE_DRAG				0.98

#define	GRAVITY					1

#define MAX_ATTRACTION			10
#define MIN_ATTRACTION			3

#define SECTOR_COUNT			1		// currently there is a bug at sector borders

bool mouseAttract	= false;
bool doMouseXY		= false;		// pressing left mmouse button moves mouse in XY plane
bool doMouseYZ		= false;		// pressing right mouse button moves mouse in YZ plane
bool doRender		= true;
int forceTimer		= false;

float rotSpeed		= 2.;
float mouseMass		= 1;

static int width;
static int height;

msa::physics::World3D		physics;
msa::physics::Particle3D		mouseNode;

ofImage					ballImage;

void initScene() {
  // clear all particles and springs etc
  physics.clear();
	
  // you can add your own particles to the physics system
  // physics.addParticle(&mouseNode);
  // mouseNode.makeFixed();
  // mouseNode.setMass(MIN_MASS);
  // mouseNode.moveTo(ofVec3f(0, 0, 0));
  // mouseNode.setRadius(NODE_MAX_RADIUS);
  ofVec3f P2(0.*width,.5*height,0.*width);
  ofVec3f P1(.3*width,1.*height,.0*width);
  ofVec3f P3(.0*width,1.*height,.3*width);
  physics.makeWall(P1,P2,P3);
  ofVec3f P5(0.*width,.5*height,.0*width);
  ofVec3f P4(.0*width,1.*height,.3*width);
  ofVec3f P6(-.3*width,1.*height,.0*width);
  physics.makeWall(P4,P5,P6);
  ofVec3f P8(0.*width,.5*height,.0*width);
  ofVec3f P7(-.3*width,1.*height,.0*width);
  ofVec3f P9(.0*width,1.*height,-.3*width);
  physics.makeWall(P7,P8,P9);
  ofVec3f P11(0.*width,.5*height,.0*width);
  ofVec3f P10(.0*width,1.*height,-.3*width);
  ofVec3f P12(.3*width,1.*height,.0*width);
  physics.makeWall(P10,P11,P12);
}
void addRandomParticle() {
  float posX		= ofRandom(-width/2, width/2);
  float posY		= ofRandom(0, height);
  float posZ		= ofRandom(-width/2, width/2);
  float mass		= ofRandom(MIN_MASS, MAX_MASS);
  float bounce	= ofRandom(MIN_BOUNCE, MAX_BOUNCE);
  float radius	= ofMap(mass, MIN_MASS, MAX_MASS, NODE_MIN_RADIUS, NODE_MAX_RADIUS);
	
  // physics.makeParticle returns a particle pointer so you can customize it
  msa::physics::Particle3D *p = physics.makeParticle(ofVec3f(posX, posY, posZ));
	
  // and set a bunch of properties (you don't have to set all of them, there are defaults)
  p->setMass(mass)->setBounce(bounce)->setRadius(radius)->enableCollision()->makeFree();
	
  // add an attraction to the mouseNode
  // if(mouseAttract) physics.makeAttraction(&mouseNode, p, ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
}
void killRandomParticle() {
  msa::physics::Particle3D *p = physics.getParticle(floor(ofRandom(0, physics.numberOfParticles())));
  // if(p && p != &mouseNode) p->kill();
}
void killRandomConstraint() {
  msa::physics::Constraint3D *c = physics.getConstraint(floor(ofRandom(0, physics.numberOfConstraints())));
  if(c) c->kill();
}
void toggleMouseAttract() {
  // mouseAttract = !mouseAttract;
  // if(mouseAttract) {
  //   // loop through all particles and add attraction to mouse
  //   // (doesn't matter if we attach attraction from mouse-mouse cos it won't be added internally
  //   for(int i=0; i<physics.numberOfParticles(); i++) physics.makeAttraction(&mouseNode, physics.getParticle(i), ofRandom(MIN_ATTRACTION, MAX_ATTRACTION));
  // } else {
  //   // loop through all existing attractsions and delete them
  //   for(int i=0; i<physics.numberOfAttractions(); i++) physics.getAttraction(i)->kill();
  // }
}

void addRandomForce(float f) {
  forceTimer = f;
  // mouseNode.addVelocity(ofVec3f(ofRandom(-f, f), ofRandom(-f, f), ofRandom(-f, f)));
  for(int i=0; i<physics.numberOfParticles(); i++) {
    msa::physics::Particle3D *p = physics.getParticle(i);
    if(p->isFree()) p->addVelocity(ofVec3f(ofRandom(-f, f), ofRandom(-f, f), ofRandom(-f, f)));
  }
}

void lockRandomParticles() {
  for(int i=0; i<physics.numberOfParticles(); i++) {
    msa::physics::Particle3D *p = physics.getParticle(i);
    if(ofRandom(0, 100) < FIX_PROBABILITY) p->makeFixed();
    else p->makeFree();
  }
  // mouseNode.makeFixed();
}

void unlockRandomParticles() {
  for(int i=0; i<physics.numberOfParticles(); i++) {
    msa::physics::Particle3D *p = physics.getParticle(i);
    p->makeFree();
  }
  // mouseNode.makeFixed();
}
//--------------------------------------------------------------
void testApp::setup(){
  ofBackground(255, 255, 255);
  ofSetVerticalSync(true);
  // ofSetFrameRate(60);
  ofSetFrameRate(25);

  //React.setup();
  soundStream.listDevices();
  bufferSize = 256;
  soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
  left.assign(bufferSize, 0.0);
  // soundStream.stop();
  
  ofDisableArbTex();
  ofEnableAlphaBlending();
  ballImage.loadImage("ball.png");
  // ballImage.loadImage("dot.png");

  width = ofGetWidth();
  height = ofGetHeight();
	
  //	physics.verbose = true;			// dump activity to log
  physics.setGravity(ofVec3f(0, GRAVITY, 0));
	
  // set world dimensions, not essential, but speeds up collision
  physics.setWorldSize(ofVec3f(-width/2, -height, -width/2), ofVec3f(width/2, height, width/2));
  physics.setSectorCount(SECTOR_COUNT);
  physics.setDrag(0.97f);
  physics.setDrag(1);		// FIXTHIS
  physics.enableCollision();
	
  initScene();
	
  // setup lighting
  GLfloat mat_shininess[] = { 50.0 };
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_position[] = { 0, height/2, 0.0, 0.0 };
  glShadeModel(GL_SMOOTH);
	
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHT0);
	
  // enable back-face culling (so we can see through the walls)
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  CutOff = 500.;
  for(int i=0;i<100;i++) addRandomParticle();
}



//--------------------------------------------------------------
void testApp::update() {
  float t = (ofGetElapsedTimef()) * 0.9f;
  float f = forceTimer;
  width = ofGetWidth();
  height = ofGetHeight();
  
  //mouseNode._pos += mouseNode._vel*0.001;
  // ofVec3f NewPos(ofRandom(-f, f),ofRandom(-f, f),ofRandom(-f, f));
  // mouseNode.moveBy(NewPos,false);

  physics.update();
  if(fabs(formerVol - scaledVol) > .01) addRandomForce(FORCE_AMOUNT);
  //React.update();
}


//--------------------------------------------------------------
void testApp::draw() {
  if(doRender) {
    string info = ofToString(ofGetFrameRate(), 2)+"\n";
    info += "Bond Length: "+ofToString(CutOff) + "\n";
    ofDrawBitmapStringHighlight(info, 0, 0);
		
    ofEnableAlphaBlending();
    glEnable(GL_DEPTH_TEST);
		
    glPushMatrix();
		
    glTranslatef(width/2, 0, -width / 3);		// center scene
    static float rot = 0;
    glRotatef(rot, 0, 1, 0);			// rotate scene
    rot += rotSpeed;						// slowly increase rotation (to get a good 3D view)
    // cam.begin();
		
    if(forceTimer) {
      float translateMax = forceTimer;
      glTranslatef(ofRandom(-translateMax, translateMax), ofRandom(-translateMax, translateMax), ofRandom(-translateMax, translateMax));
      forceTimer--;
    }
		
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    // draw right wall
    glColor3f(.9, 0.9, 0.9);		glVertex3f(width/2, height+1, width/2);
    glColor3f(1, 1, 1);				glVertex3f(width/2, -height, width/2);
    glColor3f(0.95, 0.95, 0.95);	glVertex3f(width/2, -height, -width/2);
    glColor3f(.85, 0.85, 0.85);		glVertex3f(width/2, height+1, -width/2);
		
    // back wall
    glColor3f(.9, 0.9, 0.9);		glVertex3f(width/2, height+1, -width/2);
    glColor3f(1, 1, 1);				glVertex3f(width/2, -height, -width/2);
    glColor3f(0.95, 0.95, 0.95);	glVertex3f(-width/2, -height, -width/2);
    glColor3f(.85, 0.85, 0.85);		glVertex3f(-width/2, height+1, -width/2);
		
    // left wall
    glColor3f(.9, 0.9, 0.9);		glVertex3f(-width/2, height+1, -width/2);
    glColor3f(1, 1, 1);				glVertex3f(-width/2, -height, -width/2);
    glColor3f(0.95, 0.95, 0.95);	glVertex3f(-width/2, -height, width/2);
    glColor3f(.85, 0.85, 0.85);		glVertex3f(-width/2, height+1, width/2);
		
    // front wall
    glColor3f(0.95, 0.95, 0.95);	glVertex3f(width/2, -height, width/2);
    glColor3f(.85, 0.85, 0.85);		glVertex3f(width/2, height+1, width/2);
    glColor3f(.9, 0.9, 0.9);		glVertex3f(-width/2, height+1, width/2);
    glColor3f(1, 1, 1);				glVertex3f(-width/2, -height, width/2);
		
    // floor
    glColor3f(.8, 0.8, 0.8);
    glVertex3f(width/2, height+1, width/2);
    glVertex3f(width/2, height+1, -width/2);
    glVertex3f(-width/2, height+1, -width/2);
    glVertex3f(-width/2, height+1, width/2);
    glEnd();
		
    //glEnable(GL_LIGHTING);
		
    // draw springs

    glColor4f(.8, 0.2, 0.7, 0.5);
    ofPushStyle();
    ofPushMatrix();
    ofSetLineWidth(3);
    // ofBeginShape();
    glBegin(GL_LINES);
    float VCutOff = SQR(CutOff*scaledVol);
    long np = physics.numberOfParticles();
    for(int i=0;i<np-1;i++){
      msa::physics::Particle3D *a = physics.getParticle(i);
      for(int j=i+1;j<np;j++){
	msa::physics::Particle3D *b = physics.getParticle(j);
	ofVec3f vec = b->getPosition() - a->getPosition();
	float dist = vec.lengthSquared();
	if(dist > VCutOff) continue;
	glPushMatrix();
	glVertex3f(a->getPosition().x, a->getPosition().y, a->getPosition().z);
	glVertex3f(b->getPosition().x, b->getPosition().y, b->getPosition().z);
	glPopMatrix();
      }
    }
    glEnd();
    // ofEndShape(false);
    ofPopMatrix();
    ofPopStyle();

		
    // draw particles
    glAlphaFunc(GL_GREATER, 0.5);
		
    ofEnableNormalizedTexCoords();
    ballImage.getTextureReference().bind();
    for(int i=0; i<physics.numberOfParticles(); i++) {
      msa::physics::Particle3D *p = physics.getParticle(i);
      if(p->isFixed()) glColor4f(1, 0, 0, 1);
      else glColor4f(.1, .3, 1, 1);
      
      glEnable(GL_ALPHA_TEST);
      // draw ball
      glPushMatrix();
      glTranslatef(p->getPosition().x, p->getPosition().y, p->getPosition().z);
      glRotatef(180-rot, 0, 1, 0);

      glBegin(GL_QUADS);
      glTexCoord2f(0, 0); glVertex2f(-p->getRadius(), -p->getRadius());
      glTexCoord2f(1, 0); glVertex2f(p->getRadius(), -p->getRadius());
      glTexCoord2f(1, 1); glVertex2f(p->getRadius(), p->getRadius());
      glTexCoord2f(0, 1); glVertex2f(-p->getRadius(), p->getRadius());
      glEnd();
      glPopMatrix();
			
      glDisable(GL_ALPHA_TEST);
			
      float alpha = ofMap(p->getPosition().y, -height * 1.5, height, 0, 1);
      if(alpha>0) {
	glPushMatrix();
	glTranslatef(p->getPosition().x, height, p->getPosition().z);
	glRotatef(-90, 1, 0, 0);
	glColor4f(0, 0, 0, alpha * alpha * alpha * alpha);
	//				ofCircle(0, 0, p->getRadius());
	float r = p->getRadius() * alpha;
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-r, -r);
	glTexCoord2f(1, 0); glVertex2f(r, -r);
	glTexCoord2f(1, 1); glVertex2f(r, r);
	glTexCoord2f(0, 1); glVertex2f(-r, r);
	glEnd();
	glPopMatrix();
      }
			
    }
    ballImage.getTextureReference().unbind();
    ofDisableNormalizedTexCoords();
    // draw walls
    glDisable(GL_LIGHTING);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for(int i=0; i<physics.numberOfWalls(); i++) {
      msa::physics::Wall3D *w = physics.getWall(i);
      ofVec3f P1 = w->getVertex(1);
      ofVec3f P2 = w->getVertex(2);
      ofVec3f P3 = w->getVertex(3);
      ofVec3f P4 = w->getVertex(4);      
      // draw wall
      //glBegin(GL_TRIANGLES_FAN);
      glBegin(GL_POLYGON);
      glPushMatrix();
      glColor3f(.3, 0.3, 0.3);
      glVertex3f(P1.x,P1.y,P1.z);
      glColor3f(.5, .5, .5);
      glVertex3f(P2.x,P2.y,P2.z);
      glColor3f(0.65, 0.65, 0.65);
      glVertex3f(P3.x,P3.y,P3.z);
      // glColor3f(.75, 0.75, 0.75);
      // glVertex3f(P4.x,P4.y,P4.z);
      glPopMatrix();
      glEnd();//QUADS
    }


  // cam.end();
  glPopMatrix();
	
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glColor4f(0, 0, 0, 1);
  ofDrawBitmapString(" FPS: " + ofToString(ofGetFrameRate(), 2)
		     + " | Number of particles: " + ofToString(physics.numberOfParticles(), 2)
		     + " | Number of springs: " + ofToString(physics.numberOfSprings(), 2)
		     // + " | Mouse Mass: " + ofToString(mouseNode.getMass(), 2)
		     + "\nLook at source code keyPressed to see keyboard shortcuts"
		     , 20, 15);
  }
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
  switch(key) {
  // case 'a': toggleMouseAttract(); break;
  case 'b': CutOff += 5.; break;
  case 'B': CutOff -= 5.; break;
  case 'p': for(int i=0; i<100; i++) addRandomParticle(); break;
  case 'P': for(int i=0; i<100; i++) killRandomParticle(); break;
  case 'c': physics.isCollisionEnabled() ? physics.disableCollision() : physics.enableCollision(); break;
  case 'C': killRandomConstraint(); break;
  case 'r': doRender ^= true; break;
  case 'f': addRandomForce(FORCE_AMOUNT); break;
  case 'F': addRandomForce(FORCE_AMOUNT * 3); break;
  case 'l': lockRandomParticles(); break;
  case 'u': unlockRandomParticles(); break;
  case ' ': initScene(); break;
  case 'x': doMouseXY = true; break;
  case 'z': doMouseYZ = true; break;
  case ']': rotSpeed += 0.01f; break;
  case '[': rotSpeed -= 0.01f; break;
  // case '+': mouseNode.setMass(mouseNode.getMass() +0.1); break;
  // case '-': mouseNode.setMass(mouseNode.getMass() -0.1); break;
  // case 'm': mouseNode.hasCollision() ? mouseNode.disableCollision() : mouseNode.enableCollision();
  }
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){
  switch(key) {
  case 'x': doMouseXY = false; break;
  case 'z': doMouseYZ = false; break;
  }
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ) {
  // static int oldMouseX = -10000;
  // static int oldMouseY = -10000;
  // int velX = x - oldMouseX;
  // int velY = y - oldMouseY;
  // if(doMouseXY) mouseNode.moveBy(ofVec3f(velX, velY, 0));
  // if(doMouseYZ) mouseNode.moveBy(ofVec3f(velX, 0, velY));
  // oldMouseX = x;
  // oldMouseY = y;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
  switch(button) {
  case 0:	doMouseXY = true; mouseMoved(x, y); break;
  case 2: doMouseYZ = true; mouseMoved(x, y); break;
  }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
  doMouseXY = doMouseYZ = false;
}

void testApp::audioIn(float * input, int bufferSize, int nChannels){		
  formerVol = scaledVol;
  //React.SoundWave(input,bufferSize,nChannels);
  float curVol = 0.0;
  // samples are "interleaved"
  int numCounted = 0;
  //lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
  for (int i = 0; i < bufferSize; i++){
    left[i] = input[i*2]*0.5;
    float right = input[i*2+1]*0.5;
    curVol += left[i] * left[i];
    curVol += right * right;
    numCounted += 2;
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
