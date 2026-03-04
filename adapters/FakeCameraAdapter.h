#pragma once

#include "CameraAdapter.h"
#include <vector>

class FakeCameraAdapter : public CameraAdapter {
public:
  FakeCameraAdapter();
  ~FakeCameraAdapter();
  
  bool begin() override;
  bool captureFrame(ImageFrame& frame, uint32_t& frameTimestampMs) override;
  void releaseFrame(ImageFrame& frame) override;
  void end() override;
  const char* getName() const override { return "FakeCameraAdapter"; }
  
  void loadTestImage(const uint8_t* data, int width, int height);

private:
  std::vector<uint8_t> testImage;
  int imageWidth = 0;
  int imageHeight = 0;
  uint32_t frameCount = 0;
};
