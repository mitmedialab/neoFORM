//
//  EquationMode.hpp
//  neoForm
//
//  Created by Liam May on 6/25/24.
//

#ifndef EquationMode_hpp
#define EquationMode_hpp

#include <stdio.h>
#include "Application.hpp"

class EquationMode : public Application {
public:
    EquationMode(SerialShapeIOManager *theCustomShapeDisplayManager);
    
    void setup();
    float equation1(float x, float y);
    float equation2(float x, float y);
    float equation3(float x, float y);
    //...
    //void equationN();
    
    void update(float dt);
    void updateHeights();
    void rotate(float theta);
    
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);
    
    std::tuple<int, int, int> heightPixelToMapColor(int Height);
    
    int cols;
    int rows;
    
    int timeControl;
    
    float x_Rot;
    float y_Rot;
    float x_trans;
    float y_trans;
    int count;
    
    int equationNum;
    float **heights;
    float **heightsCopy;
    
    
private:
    ofPixels m_EquationPixels;
    ofImage m_EquationImage;
    ofPixels ProjectorHeightMapPixels;
    
    
};





#endif /* EquationMode_hpp */
