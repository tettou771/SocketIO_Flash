#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofLogToConsole();
    ofSetBackgroundAuto(false);
    ofSetFrameRate(30);
    ofSetWindowTitle("SocketIO Flash");
#ifdef TARGET_OS_MAC
    ofSetDataPathRoot("../Resources/data");
#endif
    
    lastFlashTime = 0;

    // gui setup
    {
        serverAddress.addListener(this, &ofApp::setServerAddress);
        arduinoPort.addListener(this, &ofApp::setArduinoPort);
        
        gui.setup();
        gui.setSize(600, 0);
        gui.setDefaultWidth(600);
        gui.setDefaultHeight(30);
        gui.add(serverAddress.setup("Server Address", "http://your.server"));
        gui.add(serverStatus.setup("Server Status", ""));
        gui.add(arduinoPort.setup("Arduino port", "COM3"));
        gui.add(comPortList.setup("COM List", "", gui.getWidth(), 100));
        gui.add(flashCountLabel.setup("Count", "0"));

        loadConfig();
    }

    // socket io setup
    {
        string sa = serverAddress;
        setServerAddress(sa);
    }
    
    // flash device setup
    {
        string ap = arduinoPort;
        setArduinoPort(ap);
        
    }
    
    wakeupDuration = 5;
    wakeup();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    if (!needsDraw) return;
    
    ofBackground(60);

    // fit window size to gui
    gui.setPosition(0, 0);
    gui.draw();

    // flash foreground
    float duration = 0.3;
    float flashColor = MAX(50, 255 * (duration + lastFlashTime - ofGetElapsedTimef()) / duration);
    ofSetColor(255, flashColor);
    ofFill();
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
        
    if (wakeupTime + wakeupDuration < ofGetElapsedTimef()) sleep();
}

void ofApp::keyPressed(int key) { wakeup(); }
void ofApp::keyReleased(int key) { wakeup(); }
void ofApp::mousePressed(int x, int y, int button) { wakeup(); }
void ofApp::windowResized(int w, int h) {
    ofSetWindowShape(gui.getWidth(), gui.getHeight());
    wakeup();
}

void ofApp::onConnection () {
    isConnected = true;
    bindEvents();
    wakeup();
}

void ofApp::bindEvents () {
    socketIO.bindEvent(flashBangEvent, "flash_bang");
    ofAddListener(flashBangEvent, this, &ofApp::onFlashBangEvent);
    
    socketIO.bindEvent(flashBtnPressedEvent, "flash_btn_pressed");
    ofAddListener(flashBtnPressedEvent, this, &ofApp::onFlashBtnPressedEvent);
}

void ofApp::setServerAddress(string &address) {
    if (address == "") return;
    if (address == connectedAddress) return;
    
    connectedAddress = address;
    
    isConnected = false;
    serverStatus = "not connected";
    
    socketIO.closeConnection();
    socketIO.setup(address);
    ofAddListener(socketIO.notifyEvent, this, &ofApp::gotEvent);
    ofAddListener(socketIO.connectionEvent, this, &ofApp::onConnection);
}

//--------------------------------------------------------------
void ofApp::gotEvent(string& name) {
    serverStatus = name;
    wakeup();
}

//--------------------------------------------------------------
void ofApp::onFlashBangEvent (ofxSocketIOData& data) {
    ofLogNotice() << "flash bang";
    flash();
}

void ofApp::onFlashBtnPressedEvent(ofxSocketIOData& data) {
    ofLogNotice() << "flash btn pressed";
}

void ofApp::loadConfig() {
    gui.loadFromFile("gui.xml");
}

void ofApp::saveConfig() {
    gui.saveToFile("gui.xml");
}

void ofApp::setArduinoPort(string &port) {
    if (arduino.isInitialized()) {
        arduino.close();
    }

    int baudRate = 115200;
    arduino.setup(port, baudRate);

    // scan com port
    string list = "\n";
    for (auto &d : arduino.getDeviceList()) {
        list += "    " + d.getDeviceName() + "\n";
    }
    float height = (1 + arduino.getDeviceList().size()) * 18;
    comPortList.setSize(comPortList.getWidth(), height);
    comPortList.setup("COM Ports", list);

    saveConfig();

    ofSetWindowShape(gui.getWidth(), gui.getHeight());
}

void ofApp::flash() {
    if (arduino.isInitialized()) {
        arduino.writeByte('1');
    }
    lastFlashTime = ofGetElapsedTimef();
    
    flashCount++;
    flashCountLabel.setup("Count", ofToString(flashCount));
    
    wakeup();
}

void ofApp::exit() {
    saveConfig();
}
