//
//  KinectMaskMaker.hpp
//  emptyExample
//
//  Created by Charles Reischer on 1/15/25.
//

#ifndef KinectMaskMaker_hpp
#define KinectMaskMaker_hpp

#include "Application.hpp"

enum Corner {
	UpperLeft,
	UpperRight,
	LowerLeft,
	LowerRight,
};

class KinectMaskMaker : public Application {
public:
	KinectMaskMaker(SerialShapeIOManager *theCustomShapeDisplayManager, KinectManagerSimple *theKinectManager);
	void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    string getName() {return "KinectMaskMaker";}
private:
    KinectManagerSimple* kinectManager;

	bool mouseIsDown;
	Corner cornerSelected;
};

#endif //KinectMaskMaker_hpp
