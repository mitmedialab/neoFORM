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
private:
	double currentTime = 0.0;
};


#endif //AmbientWave_hpp
