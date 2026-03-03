#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

/**
 * @file DetectorCore.h
 * @brief Core cascade detection algorithm
 * 
 * Implements multi-scale sliding window cascade detection.
 * Supports LBP and Haar cascades with integer math.
 */

struct FaceBox {
  int32_t x;
  int32_t y;
  int32_t width;
  int32_t height;
};

struct CascadeStage {
  int numFeatures;
  float threshold;
  struct Feature {
    int x, y, w, h;
    int featureType;
    float weight;
    int featureValue;
  } features[256];
};

struct CascadeData {
  int width;
  int height;
  int numStages;
  CascadeStage stages[20];
  float globalThreshold;
};

/**
 * @class DetectorCore
 * @brief Multi-scale cascade detector
 */
class DetectorCore {
public:
  struct Config {
    float scaleFactor = 1.1f;
    int minNeighbors = 3;
    int minFaceWidth = 60;
    int minFaceHeight = 60;
    int maxFaceWidth = 320;
    int maxFaceHeight = 240;
    bool useLBPCascade = true;
  };

  DetectorCore(const Config& cfg = Config());
  
  bool begin(const CascadeData* cascadeData = nullptr);
  
  int detect(const uint8_t* gray, int width, int height, int stride,
            std::vector<FaceBox>& faces);
  
  void end();

  CascadeData* cascadeData = nullptr;

private:
  Config config;
  bool ownsCascade = false;
  
  int evalStages(const uint8_t* gray, int x, int y, int width, int height,
                int stride, float scale);
  uint8_t getLBPFeature(const uint8_t* gray, int x, int y, int w, int h,
                       int stride, int px, int py);
};
