#pragma once

#include "CameraAdapter.h"

class ArduinoCameraAdapter : public CameraAdapter {
public:
  ArduinoCameraAdapter();
  ~ArduinoCameraAdapter();
  
  bool begin() override;
  bool captureFrame(ImageFrame& frame, uint32_t& frameTimestampMs) override;
  void releaseFrame(ImageFrame& frame) override;
  void end() override;
  const char* getName() const override { return "ArduinoCameraAdapter"; }

private:
  uint8_t* frameBuffer = nullptr;
  bool initialized = false;
};
