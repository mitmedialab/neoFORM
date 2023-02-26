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

#include "ofxOpenCv.h"

class KinectDebugApp : public Application{
public:

    KinectDebugApp(KinectManager* kinectManager);
    
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);

private:
    KinectManager* m_kinectManager;
};
#endif /* KinectDebugApp_hpp */
