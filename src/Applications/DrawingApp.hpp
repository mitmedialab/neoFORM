//
//  DrawingApp.hpp
//  neoForm
//
//  Created by Charles Reischer on 1/21/2025
//

#ifndef DrawingApp_hpp
#define DrawingApp_hpp

#include "Application.hpp"

class DrawingApp : public Application {
public:
  DrawingApp(SerialShapeIOManager *theCustomShapeDisplayManager);

  void setup();
  string getName() { return "Drawing Mode"; }

  void update(float dt);
  void updateHeights();
  void drawGraphicsForPublicDisplay(int x, int y, int width, int height);
  void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
};

#endif /* DrawingApp_hpp */
