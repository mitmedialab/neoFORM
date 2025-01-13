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

class PropagationWave : public Application {
public:
    PropagationWave(SerialShapeIOManager *theCustomShapeDisplayManager);
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    string getName() {return "Propagation Mode";}
};


#endif //PropagationWave_hpp

