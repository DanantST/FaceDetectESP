#pragma once

#include "CameraAdapter.h"

class IDFCameraAdapter : public CameraAdapter {
public:
  IDFCameraAdapter();
  ~IDFCameraAdapter();
  
  bool begin() override;
  bool captureFrame(ImageFrame& frame, uint32_t& frameTimestampMs) override;
  void releaseFrame(ImageFrame& frame) override;
  void end() override;
  const char* getName() const override { return "IDFCameraAdapter"; }

private:
  void* cameraHandle = nullptr;
  uint8_t* frameBuffer = nullptr;
  uint32_t frameBufferSize = 0;
  bool initialized = false;
  
  void configureCamera();
};
