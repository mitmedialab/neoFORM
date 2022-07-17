#include "ofMain.h"
#include "ofApp.h"
#include "AppManager.hpp"

//========================================================================
int main( ){

    ofAppGLFWWindow window;
    
	//ofSetupOpenGL(1024,768, OF_WINDOW);			// <-------- setup the GL context
    ofSetupOpenGL(&window, 1216, 768 + 300, OF_WINDOW);
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
    AppManager *manager = new AppManager();
	//ofRunApp( new ofApp());
    ofRunApp(manager);

}
