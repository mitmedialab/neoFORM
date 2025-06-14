//
//  SmallWaveApprox.hpp
//  neoFORM
//
//  Created by Charles Reischer on 6/12/25.
//

#ifndef SmallWaveApprox_hpp
#define SmallWaveApprox_hpp

#include "Application.hpp"
#include "KinectManagerSimple.hpp"

// using the partial DEs:
//   dh/dt + H(du/dx + dv/dy) = 0
//   du/dt = -g * dh/dx - ku
//   dv/dt = -g * dh/dy - kv
//
// where:
//   H is average water height
//   k is drag coefficient
//   g is gravity constant
//   h at (x, y) is difference between water height at (x, y) and H
//   u is velocity in x direction (at x, y)
//   v is velocity in y direction (at x, y)

class SmallWaveApprox : public Application {
public:
    SmallWaveApprox(SerialShapeIOManager *shapeDisplayManager, KinectManagerSimple *kinectManager);

    void update(float dt);
    void drawGraphicsForShapeDisplay(int x, int y, int width, int height);
    void mousePressed(int x, int y, int button);

    string getName() {return "Interactive Wave - Shallow + Small Approx";}
protected:
	KinectManagerSimple *m_kinectManager;
	ofImage kinectIm;
	ofImage prevKinectIm;

	const double averageHeight = 30; // H
	const double gravConst = 0.98;     // g
	const double dragConst = 0.3;     // k

	std::vector<std::vector<float>> heightDiffs;   // h
	// velocities are at borders between heights
	std::vector<std::vector<float>> xVelocities;   // u
	std::vector<std::vector<float>> yVelocities;   // v
};

# endif // SmallWaveApprox_hpp

