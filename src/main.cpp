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
	int mainWidth = layoutSettings.getValue("mainWindowWidth", 1010);
	int mainHeight = layoutSettings.getValue("mainWindowHeight", 1060);
	settings.setSize(mainWidth, mainHeight);
	settings.setPosition(glm::vec2(
		layoutSettings.getValue("mainWindowX", 0),
		layoutSettings.getValue("mainWindowY", 0)
	));
	settings.resizable = true;
    settings.title = "control window";
    auto mainWindow = ofCreateWindow(settings);
	// Needed for portrait-style dimensions (height > width)
	mainWindow->setWindowShape(mainWidth, mainHeight);

    // create display window
	std::shared_ptr<ofAppBaseWindow> displayWindow;
	if (hasPublicWindow) {
		int publicWidth = layoutSettings.getValue("publicWindowWidth", 600);
		int publicHeight = layoutSettings.getValue("publicWindowHeight", 600);
		settings.setSize(publicWidth, publicHeight);
		settings.setPosition(glm::vec2(
			layoutSettings.getValue("publicWindowX", 1020),
			layoutSettings.getValue("publicWindowY", 0)
		));
		settings.resizable = true;
    	settings.title = "public display window";
		// uncomment next line to share main's OpenGL resources with gui
		//settings.shareContextWith = mainWindow;
		displayWindow = ofCreateWindow(settings);
		// Needed for portrait-style dimensions (height > width)
		displayWindow->setWindowShape(publicWidth, publicHeight);
	} else {
		displayWindow = nullptr;
	}
    
    // create display window
	std::shared_ptr<ofAppBaseWindow> projectorWindow;
	if (hasProjectorWindow) {
		int projectorWidth = layoutSettings.getValue("projectorWindowWidth", 600);
		int projectorHeight = layoutSettings.getValue("projectorWindowHeight", 600);
		settings.setSize(projectorWidth, projectorHeight);
		settings.setPosition(glm::vec2(
			layoutSettings.getValue("projectorWindowX", 1020),
			layoutSettings.getValue("projectorWindowY", 600)
		));
		settings.resizable = true;
    	settings.title = "projector window";
		projectorWindow = ofCreateWindow(settings);
		// Needed for portrait-style dimensions (height > width)
		projectorWindow->setWindowShape(projectorWidth, projectorHeight);
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
