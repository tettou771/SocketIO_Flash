#pragma once

#include "ofMain.h"
#include "ofxSocketIO.h"
#include "ofxSocketIOData.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mousePressed(int x, int y, int button);
    void windowResized(int x, int y);
    
    // for socket io
    ofxSocketIO socketIO;
    void gotEvent(std::string& name);
    void onFlashBangEvent(ofxSocketIOData& data);
    void onFlashBtnPressedEvent(ofxSocketIOData& data);
    bool isConnected;
    void onConnection();
    void bindEvents();
    ofEvent<ofxSocketIOData&> flashBangEvent;
    ofEvent<ofxSocketIOData&> flashBtnPressedEvent;
    void setServerAddress(string &address);
    string connectedAddress;

    // GUI
    ofxPanel gui;
    ofxTextField serverAddress;
    ofxTextField serverStatus;
    ofxTextField arduinoPort;
    ofxLabel comPortList;
    ofxLabel flashCountLabel;
    void loadConfig();
    void saveConfig();

    // flash system
    ofSerial arduino;
    void setArduinoPort(string &port);
    void flash();
    float lastFlashTime;
    int flashCount;
    
    // reduce cpu resource
    void wakeup() {wakeupTime = ofGetElapsedTimef(); needsDraw = true;}
    void sleep() {needsDraw = false;}
    bool needsDraw = true;
    float wakeupTime, wakeupDuration;
    
    // app close
    void exit() override;
};
