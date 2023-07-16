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

#include "ofxXmlSettings.h"

class KinectDebugApp : public Application{
public:

    KinectDebugApp(KinectManager* kinectManager);
    
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    
    // Press a key to save the current large bounding box (hopefully for the TRANSFORM) to a text file
    void keyPressed(int key);


private:
    KinectManager* m_kinectManager;
    
    // text file open frameworks object for saving the bounding box of the rect
    ofFile m_BoundaryFile;
    
    //parameters for saving the first frame boundary box
    int m_boundary_origin_x;
    int m_boundary_origin_y;
    int m_boundary_size_x;
    int m_boundary_size_y;
    
    int m_save_flag = 0;
};
#endif /* KinectDebugApp_hpp */
