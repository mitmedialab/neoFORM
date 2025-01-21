//
//  PropagationWave.hpp
//  emptyExample
//
//  Ported by Charles Reischer on 1/13/25.
//  Ported from "TouchShapeObject.hpp," created by Ken Nakagaki on 6/16/15
//

#ifndef PropagationWave_hpp
#define PropagationWave_hpp

#include "Application.hpp"
#include "utils.hpp"

enum TouchMode {
	waveSurface,
	singleElasticSurface,
	triSurface,
};

class PropagationWave : public Application {
public:
    PropagationWave(SerialShapeIOManager *theCustomShapeDisplayManager);
	void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    string getName() {return "Propagation Mode";}

	void changeMode(TouchMode newMode) {touchMode = newMode;}

protected:
	void waveSurface();
	void singleElasticSurface();
	void triSurface();

	void addCircleToOutput(int x, int y, int radius, int amount);

protected:
	constexpr static int numFilterFrames = 5;
	constexpr static int numWaveFrames = 50;

	int simWidth;
	int simHeight;
	int activeWidth;
	int activeHeight;

	ofPixels inputPixels;
	ofPixels outputPixels;

	std::vector<std::vector<int>> differenceHeight;
	std::vector<std::vector<bool>> isTouched;

	TouchMode touchMode;

	int defaultHeight = 40;

	circularBuffer<ofPixels> storedOutputPixels;
	circularBuffer<ofPixels> storedInputPixels;
};


#endif //PropagationWave_hpp

