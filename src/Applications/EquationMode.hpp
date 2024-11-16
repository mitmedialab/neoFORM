//
//  EquationMode.hpp
//  neoForm
//
//  Created by Liam May on 6/25/24.
//

#ifndef EquationMode_hpp
#define EquationMode_hpp

#include "Application.hpp"
#include "ofColor.h"
#include <stdio.h>

class EquationMode : public Application {
public:
  EquationMode(SerialShapeIOManager *theCustomShapeDisplayManager);

  void setup();
  float equation1(float x, float y);
  float equation2(float x, float y);
  float equation3(float x, float y);
  float equation4(float x, float y);
  float equation5(float x, float y);
  float equation6(float x, float y);
  //...
  // void equationN();

  string getName() { return "Equation Mode"; }

  void update(float dt);
  void updateHeights();
  void rotate(float theta);
  void drawGraphicsForPublicDisplay(int x, int y, int width, int height);

  bool transitioning;
  bool firstFrame;
  int transitionEq1;
  int transitionEq2;
  int transitionFrameCount;
  static const int transitionDuration = 10;
  int numFrames;

  void startTransition(int newEq1, int newEq2);

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

  const ofColor graphLowColor = ofColor::darkBlue;
  const ofColor graphHighColor = ofColor::orange;
  // angles in degrees
  float graphAngle = 0.0;
  // in degrees/second
  float graphRotationSpeed = 20.0;
  const float graphHeight = 0.4;
  ofMesh graph;
};

#endif /* EquationMode_hpp */
