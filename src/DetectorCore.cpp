#include "../include/DetectorCore.h"
#include "../include/Logger.h"
#include <cmath>
#include <algorithm>

extern const CascadeData* getFaceCascade();

DetectorCore::DetectorCore(const Config& cfg)
    : cascadeData(nullptr), config(cfg), ownsCascade(false) {
}

bool DetectorCore::begin(const CascadeData* cascadeData_) {
  FD_LOG_INFO("DetectorCore", "Initializing with scale=%.2f, minNeighbors=%d",
             config.scaleFactor, config.minNeighbors);
  
  if (cascadeData_) {
    cascadeData = const_cast<CascadeData*>(cascadeData_);
  } else {
    cascadeData = const_cast<CascadeData*>(getFaceCascade());
  }
  
  if (!cascadeData) {
    FD_LOG_ERROR("DetectorCore", "No cascade data available");
    return false;
  }
  
  FD_LOG_INFO("DetectorCore", "Cascade loaded: %dx%d, %d stages",
             cascadeData->width, cascadeData->height, cascadeData->numStages);
  
  return true;
}

int DetectorCore::detect(const uint8_t* gray, int width, int height, int stride,
                        std::vector<FaceBox>& faces) {
  faces.clear();
  
  if (!cascadeData || !gray) {
    FD_LOG_ERROR("DetectorCore", "Invalid input");
    return 0;
  }
  
  int face_count = 0;
  const int cascade_width = cascadeData->width;
  const int cascade_height = cascadeData->height;
  
  float scale = 1.0f;
  while (cascade_width * scale <= width && cascade_height * scale <= height) {
    const int window_width = (int)(cascade_width * scale);
    const int window_height = (int)(cascade_height * scale);
    
    if (window_width < config.minFaceWidth || window_height < config.minFaceHeight) {
      scale *= config.scaleFactor;
      continue;
    }
    
    if (window_width > config.maxFaceWidth || window_height > config.maxFaceHeight) {
      break;
    }
    
    for (int y = 0; y + window_height <= height; y += (int)(4 * scale)) {
      for (int x = 0; x + window_width <= width; x += (int)(4 * scale)) {
        
        int stage_response = evalStages(gray, x, y, width, height, stride, scale);
        
        if (stage_response > 0) {
          FaceBox box;
          box.x = x;
          box.y = y;
          box.width = window_width;
          box.height = window_height;
          
          faces.push_back(box);
          face_count++;
          
          if (face_count >= 1 && scale > 1.5f) {
            FD_LOG_INFO("DetectorCore", "Early stop at scale %.2f after %d faces",
                       scale, face_count);
            goto done;
          }
        }
      }
    }
    
    scale *= config.scaleFactor;
  }
  
done:
  FD_LOG_INFO("DetectorCore", "Detection complete: %d faces found", face_count);
  return face_count;
}

int DetectorCore::evalStages(const uint8_t* gray, int x, int y, int width, int height,
                            int stride, float scale) {
  float total_response = 0.0f;
  
  for (int stage_idx = 0; stage_idx < cascadeData->numStages; stage_idx++) {
    const CascadeStage& stage = cascadeData->stages[stage_idx];
    float stage_response = 0.0f;
    
    for (int feat_idx = 0; feat_idx < stage.numFeatures; feat_idx++) {
      const auto& feature = stage.features[feat_idx];
      
      int fx = x + (int)(feature.x * scale);
      int fy = y + (int)(feature.y * scale);
      int fw = (int)(feature.w * scale);
      int fh = (int)(feature.h * scale);
      
      if (fx < 0 || fy < 0 || fx + fw > width || fy + fh > height) {
        continue;
      }
      
      if (feature.featureType == 1) {
        uint8_t lbp = getLBPFeature(gray, fx, fy, fw, fh, stride, 
                                   fw / 2, fh / 2);
        
        if (lbp > feature.featureValue) {
          stage_response += feature.weight;
        }
      }
    }
    
    if (stage_response < stage.threshold) {
      return 0;
    }
    
    total_response += stage_response;
  }
  
  return (total_response > cascadeData->globalThreshold) ? 1 : 0;
}

uint8_t DetectorCore::getLBPFeature(const uint8_t* gray, int x, int y, int w, int h,
                                   int stride, int px, int py) {
  if (x + px < 1 || y + py < 1 || x + px + 1 >= x + w || y + py + 1 >= y + h) {
    return 0;
  }
  
  const uint8_t center = gray[(y + py) * stride + (x + px)];
  
  uint8_t lbp = 0;
  const int neighbors[8][2] = {
    {-1, -1}, {0, -1}, {1, -1},
    {1, 0}, {1, 1}, {0, 1},
    {-1, 1}, {-1, 0}
  };
  
  for (int i = 0; i < 8; i++) {
    int nx = x + px + neighbors[i][0];
    int ny = y + py + neighbors[i][1];
    
    uint8_t neighbor = gray[ny * stride + nx];
    if (neighbor > center) {
      lbp |= (1 << i);
    }
  }
  
  return lbp;
}

void DetectorCore::end() {
  FD_LOG_INFO("DetectorCore", "Shutdown");
  if (ownsCascade && cascadeData) {
    delete cascadeData;
  }
  cascadeData = nullptr;
}
