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
	waveSurface = 0,
	singleElasticSurface = 1,
	triSurface = 2,
};

class PropagationWave : public Application {
public:
    PropagationWave(SerialShapeIOManager *theCustomShapeDisplayManager);
	void setup();
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    string getName() {return "Propagation Mode";}

protected:
	void waveSurface();
	void singleElasticSurface();
	void triSurface();

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

	int lineSize;
	int defaultHeight = 40;

	circularBuffer<ofPixels, numFilterFrames> storedOutputPixels;
	circularBuffer<ofPixels, numWaveFrames> storedInputPixels;
};


#endif //PropagationWave_hpp

