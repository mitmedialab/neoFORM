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

//used to easily disable upscale blurring of ofImages
void setImageNotBlurry(ofImage &image) {
    image.getTexture().setTextureMinMagFilter(GL_LINEAR, GL_NEAREST);
}

