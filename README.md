# no-key-board

openFrameworks apps to run no-key-board.

The program reads the webcam, turns the image on b/w according to a settable threshold and recognizes shapes with opencv. Only shapes compatibile with finger tips are considered. For each fingertip the centroid represents the pitch and the volume of the tone. Each fingertip has to be considered as an oscillator. The program handles up to ten oscillators.

The webcam should lay under a lightened glass or plexiglass opaque surface. When the finger tips are close to the surface the webcam recognize the position and plays a tone according to the program settings.

The program has a rudimental internal synth and a midi and osc interface. Options can be enabled by hitting the letters shown in the interface enclosed by squared brakets []. The synth has a rudimental envelope too. All the initial configurations can be specified in the configuration file: openFrameworks/continuumPiano/bin/data/pianoConf.txt 

openFrameworks should be downloaded and running http://openframeworks.cc/ as well as standard c++ compilers. continuumPiano should be placed under myapps/ folder and the following addons (ofxCv, ofxOpenCv, ofxMidi, ofxOsc) installed.


