//
//  AmbientWave.hpp
//  emptyExample
//
//  Created by Charles Reischer on 12/15/24.
//

#ifndef AmbientWave_hpp
#define AmbientWave_hpp

#include "Application.hpp"
#include "SerialShapeIOManager.hpp"
class AmbientWave: public Application {
public:
	AmbientWave(SerialShapeIOManager* theCustomShapeDisplayManager) : Application(theCustomShapeDisplayManager) {}

	std::string getName() {return "Ambient Wave";}
    void update(float dt);
protected:
	double wave(double x, double y, double k_x, double k_y);
protected:
	const double timescale = 5.0;
	double currentTime = 0.0;
};


#endif //AmbientWave_hpp
