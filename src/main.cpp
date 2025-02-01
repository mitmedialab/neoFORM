#include "ProjectorApp.hpp"
#include "DisplayApp.hpp"
#include "ofMain.h"
#include "ofApp.h"
#include "ofxXmlSettings.h"
#include "AppManager.hpp"
#include <memory>

//========================================================================
int main( ){
	ofxXmlSettings layoutSettings;
	layoutSettings.load("layoutSettings.xml");

	bool hasPublicWindow = layoutSettings.getValue("hasPublicWindow", false);
	bool hasProjectorWindow = layoutSettings.getValue("hasProjectorWindow", false);

    // create main window with specific location
	ofGLFWWindowSettings settings;
	settings.setSize(
		layoutSettings.getValue("mainWindowWidth", 1010), 
		layoutSettings.getValue("mainWindowHeight", 1060)
	);
	settings.setPosition(glm::vec2(
		layoutSettings.getValue("mainWindowX", 0),
		layoutSettings.getValue("mainWindowY", 0)
	));
	settings.resizable = true;
    settings.title = "control window";
    auto mainWindow = ofCreateWindow(settings);

    // create display window
	std::shared_ptr<ofAppBaseWindow> displayWindow;
	if (hasPublicWindow) {
		settings.setSize(
			layoutSettings.getValue("publicWindowWidth", 600), 
			layoutSettings.getValue("publicWindowHeight", 600)
		);
		settings.setPosition(glm::vec2(
			layoutSettings.getValue("publicWindowX", 1020),
			layoutSettings.getValue("publicWindowY", 0)
		));
		settings.resizable = true;
    	settings.title = "public display window";
		// uncomment next line to share main's OpenGL resources with gui
		//settings.shareContextWith = mainWindow;
		displayWindow = ofCreateWindow(settings);
	} else {
		displayWindow = nullptr;
	}
    
    // create display window
	std::shared_ptr<ofAppBaseWindow> projectorWindow;
	if (hasProjectorWindow) {
		settings.setSize(
			layoutSettings.getValue("projectorWindowWidth", 600), 
			layoutSettings.getValue("projectorWindowHeight", 600)
		);
		settings.setPosition(glm::vec2(
			layoutSettings.getValue("projectorWindowX", 1020),
			layoutSettings.getValue("projectorWindowY", 600)
		));
		settings.resizable = true;
    	settings.title = "projector window";
		projectorWindow = ofCreateWindow(settings);
	} else {
		projectorWindow = nullptr;
	}
    
    // seperate applications (needed for some openframeworks stuff)
    auto manager = make_shared<AppManager>();
    auto displayApp = hasPublicWindow ? make_shared<DisplayApp>(displayWindow->getWidth(), displayWindow->getHeight()) : nullptr;
    auto projectorApp = hasProjectorWindow ? make_shared<ProjectorApp>(projectorWindow->getWidth(), projectorWindow->getHeight()) : nullptr;

    // let the windows see each other
    if (hasPublicWindow) displayApp->mainApp = manager;
    manager->displayWindow = displayWindow;
    if (hasProjectorWindow) projectorApp->mainApp = manager;
    manager->projectorWindow = projectorWindow;
    manager->cam = &displayApp->cam;

    ofRunApp(mainWindow, manager);
    if (hasPublicWindow) ofRunApp(displayWindow, displayApp);
    if (hasProjectorWindow) ofRunApp(projectorWindow, projectorApp);
	ofRunMainLoop();

}
