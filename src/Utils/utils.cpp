//
//  utils.cpp
//  FinaleForm
//
//  Created by Daniel Levine on 5/5/21.
//

#include "utils.hpp"

double elapsedTimeInSeconds() {
    return clock() / (double) CLOCKS_PER_SEC;
}

ofImage applyFunctionToPixels(const ofImage& first, const ofImage& second, pixelFunction fn) {
    ofImage out = ofImage(first);

    ofPixels& outPixels = out.getPixels();
    const ofPixels& secondPixels = second.getPixels();

    // weighted average of pixel
    for (int i = 0; i < outPixels.size(); i++) {
        outPixels[i] = fn(outPixels[i], secondPixels[i]);
    }
    
    out.update();
    return out;

}

ofImage absoluteDifference(const ofImage& first, const ofImage& second) {
    return applyFunctionToPixels(first, second, [](unsigned char a, unsigned char b){return (unsigned char)abs(a - int(b));});
}

ofImage weightedAverage(const ofImage& first, const ofImage& second, double weight) {
    ofImage out = ofImage(first);
    weight = std::clamp(weight, 0.0, 1.0);

    ofPixels& outPixels = out.getPixels();
    const ofPixels& secondPixels = second.getPixels();

    // weighted average of pixel
    for (int i = 0; i < outPixels.size(); i++) {
        outPixels[i] = (1.0 - weight) * outPixels[i] + weight * secondPixels[i];
    }
    
    out.update();
    return out;
}

