// void testApp::FindFreq(void){
//   unsigned char * pixels = video.getPixels();
//   int vw = video.getWidth();
//   int vh = video.getHeight();
//   int width = ofGetWidth();
//   int height = ofGetHeight();
//   unsigned char *ImPix = pixels;
//   int ColTh = 180;
//   int Count[NPhTot];
//   int Freq[NPhTot];
//   for(int p=0;p<NPhTot;p++){
//     Count[p]=0.;
//     Freq[p] = 0.;
//   }
//   NPhase = 0;
//   int NCorrW = 20;
//   int NCorrH = 20;
//   int SliceD = vh/4;
//   int SliceU = 3*vh/4;
//   for(int h=SliceD;h<SliceU;h+= NCorrH){
//     for(int w=0;w<vw;w++){
//       for(int h1=h;h1<h+NCorrH;h1++){
// 	if(ImPix[(h1*vw+w)*3  ] > ColTh){
// 	  for(int d=0;d<3;d++) ImPix[(h1*vw+w)*3+d] = 255;
// 	  Freq[NPhase] += w;
// 	  Count[NPhase] += 1;
// 	  if(Count[NPhase] >= NCorrW*NCorrH){
// 	    if(NPhase >= NPhTot) break;
// 	    NPhase++;
// 	  }
// 	}
// 	else for(int d=0;d<3;d++) ImPix[(h1*vw+w)*3+d] = 0;
//       }
//     }
//   }
//   for(int p=0;p<NPhase;p++){
//     float Norm = Count[p] > 0 ? 1./(float)(Count[p]*vw) : 1.;
//     float Pitch = (float)Freq[p]*Norm;
//     Freq[p] = 2000.*Pitch;
//     phaseAdderTarget[p] = (Freq[p] / (float) sampleRate) * TWO_PI;
//   }
//   ofImage DrImage;
//   DrImage.setFromPixels(ImPix,vw,vh,OF_IMAGE_COLOR,false);
//   DrImage.draw(width/2-vw/2,vh/2);
// }
