//
//  Telepresence.hpp
//  emptyExample
//
//  Created by Charles Reischer on 11/22/24.
//

#ifndef Telepresence_hpp
#define Telepresence_hpp

#include "Application.hpp"

class Telepresence : public Application {
public:
	Telepresence(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManager *theKinectManager);
	void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    string getName() {return "Telepresence";}
private:
    KinectManager* kinectManager;
	ofPixels refinedImage;
};

#endif //Telepresence_hpp
