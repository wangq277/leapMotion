#include "ofApp.h"

static bool shouldRemove(shared_ptr<ofxBox2dBaseShape>shape) {
    return !ofRectangle(0, -400, ofGetWidth(), ofGetHeight()+400).inside(shape->getPosition());
}
//--------------------------------------------------------------

// relative point list  (starting at origin)
ofPoint pBot = ofPoint(0, 0);  // start
ofPoint pRi = ofPoint(50, 50); // go up and right
ofPoint pLe = ofPoint(50, -50);  // go up and left
ofPoint pTop = ofPoint(-50, -50); // go down and left
ofPoint pEnd = ofPoint(-50, 50);  // go down and right   it's a circle!
ofPoint gPoints[] = {pBot, pRi, pTop, pLe, pEnd};

simpleGesture gesture;

void ofApp::setup(){
    //---------------------box2d---------------------
//    ofDisableAntiAliasing();
//    ofBackgroundHex(0xfdefc2);
//    ofSetLogLevel(OF_LOG_NOTICE);
//    ofSetVerticalSync(true);
    //---------------------box2d end---------------------
    
    //---------------------leap motion---------------------
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
	//ofSetLogLevel(OF_LOG_VERBOSE);
    
	leap.open();
    
	cam.setOrientation(ofPoint(-20, 0, 0));
    
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    
    gesture.setup(gPoints, 4, 2000);
    //---------------------leap motion end---------------------
    
    
    //---------------------box2d---------------------
    
    
    box2d.init();
    box2d.setGravity(0,10);
    box2d.createBounds();
    box2d.setFPS(30.0);
    box2d.registerGrabbing();
    //---------------------box2d end---------------------


    

}

vector <ofPoint> ofApp::loadPoints(string file) {
    vector <ofPoint> pts;
    vector <string>  ptsStr = ofSplitString(ofBufferFromFile(file).getText(), ",");
    for(int i=0; i<ptsStr.size(); i+=2) {
        float x = ofToFloat(ptsStr[i]);
        float y = ofToFloat(ptsStr[i+1]);
        pts.push_back(ofPoint(x, y));
    }
    return pts;
}


//--------------------------------------------------------------
void ofApp::update(){
	fingersFound.clear();
	
    //-----------------box2d------------------
//    // add some circles every so often
//    if((int)ofRandom(0, 10) == 0) {
//        shared_ptr<ofxBox2dCircle> circle = shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle);
//        circle->setPhysics(0.3, 0.5, 0.1);
//        circle->setup(box2d.getWorld(), (ofGetWidth()/2)+ofRandom(-20, 20), -20, ofRandom(10, 20));
//        circles.push_back(circle);
//    }
//    
//    ofRemove(circles, shouldRemove);
    box2d.update();
    
    
    //---------------box2d end ----------------
    
    
    
	//here is a simple example of getting the hands and drawing each finger and joint
	//the leap data is delivered in a threaded callback - so it can be easier to work with this copied hand data
	
	//if instead you want to get the data as it comes in then you can inherit ofxLeapMotion and implement the onFrame method.
	//there you can work with the frame data directly.
    
    
    
    //Option 1: Use the simple ofxLeapMotionSimpleHand - this gives you quick access to fingers and palms.
    
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
	leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
        
        fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
        
        /*
        for(int i = 0; i < simpleHands.size(); i++){
            for (int f=0; f<5; f++) {
                int id = simpleHands[i].fingers[ fingerTypes[f] ].id;
                ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp; // metacarpal
                ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip; // proximal
                ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip; // distal
                ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip; // fingertip
                fingersFound.push_back(id);
            }
        }
        */
    }
    
    
    
	leap.markFrameAsOld();
}


// define your gesture
void simpleGesture::setup(ofPoint *points, int maxIndex, int msTout){
  gPts = points;
  maxIdx = maxIndex;
  timeout = msTout;
  curIdx = 1;
}

// Give a moving input point.  Returns true if gesture matched.


bool simpleGesture::track (ofPoint iPt) {
    
  bool xMatch = false, yMatch = false;
  int timePerPoint = timeout / maxIdx;
  
  int currentT = ofGetElapsedTimeMillis();
  if (currentT > stepStartTime + timePerPoint) {
    curIdx = 1;
    lastPoint = iPt;
    stepStartTime = currentT;
  }
  
  // where to go next abs screen coords
  ofPoint absNext = lastPoint + gPoints[curIdx];  
  
  if (gPoints[curIdx-1].x == gPoints[curIdx].x) {  // if nothing has changed, match.
    xMatch = true;
  }
  
  if (gPoints[curIdx-1].y == gPoints[curIdx].y) {  // if nothing has changed, match.
    yMatch = true;
  }
  
  // find out what direction we are heading in 
  if (gPoints[curIdx-1].x < gPoints[curIdx].x) {  // going right
    if (absNext.x < iPt.x)			// have we exceeded that x?
      xMatch = true;
       } else {					// going left
    if (absNext.x > iPt.x)
        
      xMatch = true;
//          ofDrawCircle(absNext.x,absNext.y , 80);
  }
  
  if (gPoints[curIdx-1].y < gPoints[curIdx].y) {  // going down
    if (absNext.y < iPt.y) 			// have we exceeded that y?
      yMatch = true;
     
//      ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255));
//      ofFill();
//       ofDrawCircle(absNext.x, absNext.y, 20);
//      for(int i = 0; i<20;i ++){
//      ofDrawRectangle(absNext.x+200*i, absNext.y+200*i, 30, 25);
//      }
  } else {					// going up
    if (absNext.y > iPt.y)
      yMatch = true;
//       ofSetColor(ofRandom(255), ofRandom(255), ofRandom(255));
//      ofDrawCircle(absNext.x, absNext.y, 20);
      
 
  }
  
  // if we have x & y
  if (xMatch && yMatch) {
    curIdx++;
    lastPoint = iPt;
    stepStartTime = currentT;
      
    
    if (curIdx == maxIdx) {  // if we just matched our last point 
      // cout << "Matched" << endl;  // debug
      curIdx = 1; // start from beginning
      return(true);
    

    }
  }
  
  // cout << iPt.x << " " << iPt.y << " " << curIdx << endl;  // debug
  return (false);
    
    
    
    
    }

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30),  OF_GRADIENT_BAR);
	
	ofSetColor(200);
	ofDrawBitmapString("ofxLeapMotion - Example App\nLeap Connected? " + ofToString(leap.isConnected()), 20, 20);
    
    
	cam.begin();
    
	ofPushMatrix();
    //ofRotate(90, 0, 0, 1);
    ofSetColor(20);
    //ofDrawGridPlane(800, 20, false);
	ofPopMatrix();
    
    //---------------box2d-----------------
    ofSetColor(255, 255, 255);
    for (int i = 0; i <circles.size (); i ++){
        ofPoint pos = circles [i] .get () -> getPosition ();
        circles [i] .get () -> draw ();
    }
//    for(int i=0; i<circles.size(); i++) {
//        ofFill();
//        ofSetHexColor(0xc0dd3b);
//        circles[i]->draw();
//    }
//    
//    ofSetHexColor(0x444342);
//    ofFill();
//    shape.draw();
//    
//    ofSetHexColor(0x444342);
//    ofNoFill();
//    for(int i=0; i<polyShapes.size(); i++) {
//        polyShapes[i]->draw();
//        
//        ofCircle(polyShapes[i]->getPosition(), 3);
//    }
//    
//    // some debug information
//    string info = "Draw a shape with the mouse\n";
//    info += "Press 1 to add some circles\n";
//    info += "Press c to clear everything\n";
//    info += "Press t to break object up into triangles/convex poly: "+string(breakupIntoTriangles?"true":"false")+"\n";
//    info += "Total Bodies: "+ofToString(box2d.getBodyCount())+"\n";
//    info += "Total Joints: "+ofToString(box2d.getJointCount())+"\n\n";
//    info += "FPS: "+ofToString(ofGetFrameRate())+"\n";
//    ofSetHexColor(0x444342);
//    ofDrawBitmapString(info, 10, 15);
//    
     //---------------box2d end-----------------
    
    fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
    
    for(int i = 0; i < simpleHands.size(); i++){
        bool isLeft        = simpleHands[i].isLeft;
        ofPoint handPos    = simpleHands[i].handPos;
        ofPoint handNormal = simpleHands[i].handNormal;
        ofSetColor(255);
        
        ofSetColor(0, 0, 255);
        ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
        ofSetColor(255, 255, 0);
        ofDrawArrow(handPos, handPos + 100*handNormal, 20);
        cout<< " number" << handNormal.y<< endl;
        
//        ofSetColor(ofRandom(100,200), ofRandom(100,200), ofRandom(100,200));
//
        if (handPos.y<-200){
            float r = ofRandom(4,10);
//            ofSetColor(ofRandom(100,200), ofRandom(100,200), ofRandom(100,200));
            

            circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
            circles.back().get()->setPhysics(3.0, 0.6, 10);
            circles.back().get()->setup(box2d.getWorld(),handPos.x,handPos.y,r);
            
        }

       
        
        // Give simple gesture more data and get return value
        bool gDone = gesture.track(simpleHands[0].fingers[INDEX].tip);  // send index finger tip
        if (gDone)
        
          cout << "----------------------GOT IT----------------------" << endl;
        
        for (int f=0; f<5; f++) {
            ofPoint mcp = simpleHands[i].fingers[ fingerTypes[f] ].mcp;  // metacarpal
            ofPoint pip = simpleHands[i].fingers[ fingerTypes[f] ].pip;  // proximal
            ofPoint dip = simpleHands[i].fingers[ fingerTypes[f] ].dip;  // distal
            ofPoint tip = simpleHands[i].fingers[ fingerTypes[f] ].tip;  // fingertip
            
            ofSetColor(0, 255, 0);
            ofDrawSphere(mcp.x, mcp.y, mcp.z, 12);
            ofDrawSphere(pip.x, pip.y, pip.z, 12);
            ofDrawSphere(dip.x, dip.y, dip.z, 12);
            ofDrawSphere(tip.x, tip.y, tip.z, 12);
           // cout<< " number tip: " <<  tip.x<<" number dip: " <<dip.x<<" number pip: " <<pip.x<< " number mpc: " <<mcp.x<< endl;
            
            ofSetColor(255, 0, 0);
            ofSetLineWidth(20);
            ofLine(mcp, pip);
            ofLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
            ofLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
            
//            if (handNormal.y>0){
//                float r = ofRandom(1,3);
//                circles.push_back(shared_ptr<ofxBox2dCircle>(new ofxBox2dCircle));
//                circles.back().get()->setPhysics(3.0, 0.6, 10);
//                circles.back().get()->setup(box2d.getWorld(),handPos.x,handPos.y,r);
//                
//            }

          // tip.x + pip.x <-300 || tip.x + pip.x >400
//            fingerType fingerTypes[] = {THUMB, INDEX, MIDDLE, RING, PINKY};
//            ofPoint gPoints[] = {pBot, pRi, pTop, pLe, pEnd};
           
        }
       

        
    }
	cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    
  

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}

//--------------------------------------------------------------
void ofApp::exit(){
    // let's close down Leap and kill the controller
    leap.close();
}
