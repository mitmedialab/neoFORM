//
//  KinectDebugApp.hpp
//  neoForm
//
//  Created by admin on 10/18/22.
//

#ifndef KinectDebugApp_hpp
#define KinectDebugApp_hpp

#include <stdio.h>
#include "Application.hpp"
#include "KinectManager.hpp"

class KinectDebugApp : public Application{
public:
//    KinectDebugApp();
//    ~KinectDebugApp();
    
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    // near and far boundary values for depth data captured, specified in millimeters
    //pair<int, int> getDepthInputBoundaries();
};

#endif /* KinectDebugApp_hpp */
