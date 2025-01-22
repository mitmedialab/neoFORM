//
//  DrawingApp.hpp
//  neoForm
//
//  Created by Charles Reischer on 1/21/2025
//

#ifndef DrawingApp_hpp
#define DrawingApp_hpp

#include "Application.hpp"
#include "utils.hpp"

struct DrawingPoint {
	int x;
	int y;
	float age; // in seconds
	bool isStart; // start of a given brush stroke
};

class DrawingApp : public Application {
public:
	DrawingApp(SerialShapeIOManager *theCustomShapeDisplayManager);
	
	string getName() { return "Drawing Mode"; }
	
	void update(float dt);
	void updateHeights();
	//void drawGraphicsForPublicDisplay(int x, int y, int width, int height);
	//void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
protected:
	void leftMousePressed();
protected:
	circularBuffer<DrawingPoint> drawnPoints;
	int framesSinceLastMousePress = 2;

	const float totalPointLifespan = 10.0; // time for a drawn point to disapear completely
	const float fullIntensityLifespan = 6.0; // time before a drawn point starts to fade

	const int mainDisplayX = 400;
	const int mainDisplayY = 356;
	const int mainDisplayWidth = 600;
	const int mainDisplayHeight = 600;
};

#endif /* DrawingApp_hpp */
