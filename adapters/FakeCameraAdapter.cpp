#include "FakeCameraAdapter.h"
#include "../include/Logger.h"
#include <cstring>

static const char* TAG = "FakeCameraAdapter";

FakeCameraAdapter::FakeCameraAdapter()
    : imageWidth(0), imageHeight(0), frameCount(0) {
}

FakeCameraAdapter::~FakeCameraAdapter() {
  end();
}

bool FakeCameraAdapter::begin() {
  FD_LOG_INFO(TAG, "Fake camera initialized");
  
  if (testImage.empty()) {
    imageWidth = 320;
    imageHeight = 240;
    testImage.resize(imageWidth * imageHeight);
    
    for (int y = 0; y < imageHeight; y++) {
      for (int x = 0; x < imageWidth; x++) {
        uint8_t val = 128;
        
        if (x >= 130 && x < 190 && y >= 90 && y < 150) {
          val = 200;
        }
        
        testImage[y * imageWidth + x] = val;
      }
    }
    
    FD_LOG_INFO(TAG, "Generated default 320x240 test image with face pattern");
  }
  
  return true;
}

bool FakeCameraAdapter::captureFrame(ImageFrame& frame, uint32_t& frameTimestampMs) {
  if (testImage.empty()) {
    FD_LOG_ERROR(TAG, "No test image loaded");
    return false;
  }
  
  frameTimestampMs = millis();
  frameCount++;
  
  frame.data = testImage.data();
  frame.width = imageWidth;
  frame.height = imageHeight;
  frame.stride = imageWidth;
  frame.format = ImageFrame::GRAY8;
  
  FD_LOG_DEBUG(TAG, "Frame %u captured (%dx%d)", frameCount, imageWidth, imageHeight);
  
  return true;
}

void FakeCameraAdapter::releaseFrame(ImageFrame& frame) {
}

void FakeCameraAdapter::end() {
  FD_LOG_INFO(TAG, "Fake camera shutdown");
  testImage.clear();
}

void FakeCameraAdapter::loadTestImage(const uint8_t* data, int width, int height) {
  if (!data || width <= 0 || height <= 0) {
    FD_LOG_ERROR(TAG, "Invalid test image data");
    return;
  }
  
  imageWidth = width;
  imageHeight = height;
  testImage.assign(data, data + width * height);
  
  FD_LOG_INFO(TAG, "Test image loaded: %dx%d", width, height);
}
