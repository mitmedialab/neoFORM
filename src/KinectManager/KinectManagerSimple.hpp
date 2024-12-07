//
//  KinectManagerSimple.hpp
//  neoForm
//
//  Created by Charles Reischer on 12/7/24.
//

#include <stdio.h>
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxXmlSettings.h"

class KinectManagerSimple {
public:
	KinectManagerSimple(short nearClip = 500, short farClip = 3800);
    ~KinectManagerSimple();
    void update();
	ofShortPixels getDepthPixels();
	ofPixels getColorPixels();
	void crop(ofImage &pix);
	void crop(ofShortImage &pix);
	void setDepthClipping(short nearClip, short farClip);
	void thresholdInterp(ofShortPixels &pix, unsigned short lowThresh, unsigned short highThresh, unsigned short lowValue, unsigned short highValue);

protected:
	bool is_connected;
	ofRectangle mask;
	ofxKinect kinect;
	ofShortPixels depthPixels;
	ofPixels colorPixels;
};
