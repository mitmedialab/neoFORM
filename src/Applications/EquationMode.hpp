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
  // opengl resources are specific to ofApp, so for example images must be loaded in the same ofApp they are drawn in
  virtual void publicDisplaySetup();

  // declares an alias of (the type of) member function pointers matching the equations
  typedef float (EquationMode::*EquationPointer)(float, float);

  float runCurrentEq(float x, float y);
  float equation1(float x, float y);
  float equation2(float x, float y);
  float equation3(float x, float y);
  float equation4(float x, float y);
  float equation5(float x, float y);
  //...
  // void equationN();

  static const int numEquations = 5;

  std::vector<ofImage> equationImages;

  // allows easier use of equations (yes the "EquationMode::" is necessary)
  EquationPointer equations[numEquations] = {
	  &EquationMode::equation1,
	  &EquationMode::equation2,
	  &EquationMode::equation3,
	  &EquationMode::equation4,
	  &EquationMode::equation5,
	  //...
	  // &EquationMode::equationN,
  };

  string getName() { return "Equation Mode"; }

  void update(float dt);
  void updateHeights();
  void rotate(float theta);
  void drawGraphicsForPublicDisplay(int x, int y, int width, int height);
  void drawGraphicsForProjector(int x, int y, int width, int height);

  bool transitioning;
  bool firstFrame;
  int transitionEq1 = 0;
  int transitionEq2 = 0;
  int transitionFrameCount;
  int numFrames;

  void startTransition(int newEq1, int newEq2);

  void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
  void drawSectionPreviewFrameBuffer(int x, int y, int width, int height);

  ofColor heightPixelToMapColor(int Height);

  int cols;
  int rows;

  int timeControl;

  float x_Rot;
  float y_Rot;
  float x_trans;
  float y_trans;
  int count;

  int equationIndex = 0;
  float **heights;
  float **heightsCopy;

private:
  ofPixels m_EquationPixels;
  ofImage m_EquationImage;
  ofPixels ProjectorHeightMapPixels;

  const int graphDetailMultple = 2;
  // angles in degrees
  float graphAngle = 0.0;
  // in degrees/second
  float graphRotationSpeed = 20.0;
  const float graphHeight = 0.4;
  ofMesh graph;
  ofMesh graphBacking;
  float backingDistance = 0.01;
};

#endif /* EquationMode_hpp */
