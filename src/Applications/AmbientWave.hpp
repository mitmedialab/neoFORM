//
//  AmbientWave.hpp
//  emptyExample
//
//  Created by Charles Reischer on 12/15/24.
//

#ifndef AmbientWave_hpp
#define AmbientWave_hpp

struct Boat {
	double x;
	double y;
};

#include "Application.hpp"
#include "SerialShapeIOManager.hpp"
class AmbientWave: public Application {
public:
	AmbientWave(SerialShapeIOManager* theCustomShapeDisplayManager);

	std::string getName() {return "Ambient Wave";}
    void update(float dt);

protected:
	double wave(double x, double y, double k_x, double k_y);
	std::pair<double, double> gradientOfWave(double x, double y, double k_x, double k_y, double phase = 0);

protected:
	const double timescale = 5.0;
	double currentTime = 0.0;

	//std::array<Boat, 5> boats;
};


#endif //AmbientWave_hpp
