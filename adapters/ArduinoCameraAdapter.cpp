#include "ArduinoCameraAdapter.h"
#include "../include/Logger.h"
#include <Arduino.h>

static const char* TAG = "ArduinoCameraAdapter";

ArduinoCameraAdapter::ArduinoCameraAdapter()
    : frameBuffer(nullptr), initialized(false) {
}

ArduinoCameraAdapter::~ArduinoCameraAdapter() {
  end();
}

bool ArduinoCameraAdapter::begin() {
  FD_LOG_INFO(TAG, "Initializing Arduino camera adapter...");
  
  frameBuffer = (uint8_t*)malloc(320 * 240);
  if (!frameBuffer) {
    FD_LOG_ERROR(TAG, "Failed to allocate frame buffer");
    return false;
  }
  
  #ifdef ARDUINO_CAMERA_AVAILABLE
  FD_LOG_INFO(TAG, "Arduino camera API available");
  #else
  FD_LOG_WARN(TAG, "Arduino camera API not available; using test data");
  #endif
  
  initialized = true;
  FD_LOG_INFO(TAG, "Arduino camera adapter ready");
  
  return true;
}

bool ArduinoCameraAdapter::captureFrame(ImageFrame& frame, uint32_t& frameTimestampMs) {
  if (!initialized || !frameBuffer) {
    FD_LOG_ERROR(TAG, "Adapter not initialized");
    return false;
  }
  
  frameTimestampMs = millis();
  
  for (int i = 0; i < 320 * 240; i++) {
    frameBuffer[i] = (uint8_t)(i % 256);
  }
  
  frame.data = frameBuffer;
  frame.width = 320;
  frame.height = 240;
  frame.stride = 320;
  frame.format = ImageFrame::GRAY8;
  
  return true;
}

void ArduinoCameraAdapter::releaseFrame(ImageFrame& frame) {
  frame.data = nullptr;
}

void ArduinoCameraAdapter::end() {
  FD_LOG_INFO(TAG, "Shutting down...");
  
  if (frameBuffer) {
    free(frameBuffer);
    frameBuffer = nullptr;
  }
  
  initialized = false;
}
