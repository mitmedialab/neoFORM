//
//  TransitionApp.hpp
//  neoForm
//
//  Created by Charles Reischer 12/14/24.
//

#ifndef TransitionApp_hpp 
#define TransitionApp_hpp 

#include "Application.hpp"
class TransitionApp : public Application {
public:
	TransitionApp() {}

	std::string getName(); 
	void startTransition(Application* start, Application* end, float duration, Application** pointerToActiveApplication);
    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void drawGraphicsForPublicDisplay(int x, int y, int width, int height);
    void drawGraphicsForProjector(int x, int y, int width, int height);
    void keyPressed(int key);
public:
	Application* startApp = nullptr;
	Application* endApp = nullptr;
	Application** appManagerPointer = nullptr;
	float transitionDuration = 0.0f;
	float transitionProgress = 0.0f;
};







#endif //TransitionApp_hpp
