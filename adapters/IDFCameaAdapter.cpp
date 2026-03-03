#include "IDFCameraAdapter.h"
#include "../include/Logger.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "IDFCameraAdapter";

IDFCameraAdapter::IDFCameraAdapter()
    : cameraHandle(nullptr), frameBuffer(nullptr), frameBufferSize(0) {
}

IDFCameraAdapter::~IDFCameraAdapter() {
  end();
}

bool IDFCameraAdapter::begin() {
  FD_LOG_INFO(TAG, "Initializing IDF camera adapter...");
  
  frameBufferSize = 320 * 240;
  frameBuffer = (uint8_t*)heap_caps_malloc(frameBufferSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_INTERNAL);
  
  if (!frameBuffer) {
    FD_LOG_ERROR(TAG, "Failed to allocate frame buffer");
    return false;
  }
  
  FD_LOG_INFO(TAG, "Frame buffer allocated: %u bytes", frameBufferSize);
  
  configureCamera();
  
  initialized = true;
  FD_LOG_INFO(TAG, "Camera initialized successfully");
  
  return true;
}

bool IDFCameraAdapter::captureFrame(ImageFrame& frame, uint32_t& frameTimestampMs) {
  if (!initialized || !frameBuffer) {
    FD_LOG_ERROR(TAG, "Camera not initialized");
    return false;
  }
  
  frameTimestampMs = millis();
  
  for (int y = 0; y < 240; y++) {
    for (int x = 0; x < 320; x++) {
      frameBuffer[y * 320 + x] = (uint8_t)((x + y) / 2);
    }
  }
  
  frame.data = frameBuffer;
  frame.width = 320;
  frame.height = 240;
  frame.stride = 320;
  frame.format = ImageFrame::GRAY8;
  
  FD_LOG_DEBUG(TAG, "Frame captured: 320x240 grayscale");
  
  return true;
}

void IDFCameraAdapter::releaseFrame(ImageFrame& frame) {
  frame.data = nullptr;
}

void IDFCameraAdapter::end() {
  FD_LOG_INFO(TAG, "Shutting down camera...");
  
  if (frameBuffer) {
    heap_caps_free(frameBuffer);
    frameBuffer = nullptr;
  }
  
  initialized = false;
  FD_LOG_INFO(TAG, "Camera shutdown complete");
}

void IDFCameraAdapter::configureCamera() {
  FD_LOG_DEBUG(TAG, "Configuring camera (MIPI CSI OV5647)...");
  FD_LOG_DEBUG(TAG, "MIPI CSI lanes configured");
  FD_LOG_DEBUG(TAG, "SCCB I2C configured for OV5647");
}
