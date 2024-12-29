
//
//  Telepresence.hpp
//  emptyExample
//
//  Created by Charles Reischer on 11/22/24.
//

#ifndef Telepresence_hpp
#define Telepresence_hpp

#include "Application.hpp"
#include "ofVideoGrabber.h"

class Telepresence : public Application {
public:
	Telepresence(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManagerSimple *theKinectManager, 
			  int maxOutDist, int bottomOutDist, ofVideoGrabber *cam);
	void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
	void drawGraphicsForPublicDisplay(int x, int y, int width, int height);
    
    string getName() {return "Telepresence";}
private:
    KinectManagerSimple* kinectManager;
	int maxOutDist;
	int bottomOutDist;
	ofImage refinedImage;
	ofVideoGrabber *cam;
};

#endif //Telepresence_hpp
