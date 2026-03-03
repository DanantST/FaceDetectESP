#include "../include/FaceDetectESP.h"
#include "../include/Logger.h"
#include <cstring>
#include <algorithm>
#include <cmath>

static const char* TAG = "FaceDetectESP";

FaceDetectESP::FaceDetectESP(CameraAdapter* camAdapter, const Config& cfg)
    : camera(camAdapter),
      detector(DetectorCore::Config{
        .scaleFactor = cfg.scaleFactor,
        .minNeighbors = cfg.minNeighbors,
        .minFaceWidth = cfg.minFaceSize,
        .minFaceHeight = cfg.minFaceSize,
        .maxFaceWidth = cfg.width,
        .maxFaceHeight = cfg.height,
        .useLBPCascade = cfg.useLBPCascade
      }),
      config(cfg),
      initialized(false) {
}

FaceDetectESP::~FaceDetectESP() {
  end();
}

bool FaceDetectESP::begin() {
  FD_LOG_INFO(TAG, "=== FaceDetectESP v2.0.0 Initialization ===");
  FD_LOG_INFO(TAG, "Config: %dx%d, scale=%.2f, minNeighbors=%d, minFaceSize=%d",
             config.width, config.height, config.scaleFactor,
             config.minNeighbors, config.minFaceSize);
  
  if (!camera) {
    FD_LOG_ERROR(TAG, "No camera adapter provided");
    return false;
  }
  
  if (!camera->begin()) {
    FD_LOG_ERROR(TAG, "Camera adapter initialization failed: %s", camera->getName());
    return false;
  }
  
  FD_LOG_INFO(TAG, "Camera initialized: %s", camera->getName());
  
  if (!detector.begin()) {
    FD_LOG_ERROR(TAG, "Detector initialization failed");
    return false;
  }
  
  initialized = true;
  detectionCount = 0;
  totalDetectionTimeMs = 0;
  
  FD_LOG_INFO(TAG, "=== Initialization Complete ===");
  return true;
}

bool FaceDetectESP::detect(FaceBox& faceBox, uint32_t* frameTimestamp) {
  if (!initialized) {
    FD_LOG_ERROR(TAG, "Not initialized; call begin() first");
    return false;
  }
  
  ImageFrame frame;
  uint32_t ts = 0;
  
  FD_LOG_DEBUG(TAG, "Capturing frame...");
  if (!camera->captureFrame(frame, ts)) {
    FD_LOG_WARN(TAG, "Frame capture failed or timeout");
    return false;
  }
  
  if (frameTimestamp) {
    *frameTimestamp = ts;
  }
  lastFrameTimestamp = ts;
  
  FD_LOG_DEBUG(TAG, "Frame received: %dx%d, format=%d, stride=%d",
              frame.width, frame.height, frame.format, frame.stride);
  
  uint8_t* gray = nullptr;
  bool ownedGray = false;
  
  if (frame.format == ImageFrame::GRAY8) {
    gray = frame.data;
  } else if (frame.format == ImageFrame::RGB565) {
    gray = convertRGB565toGray(frame.data, frame.width, frame.height);
    ownedGray = true;
    
    if (!gray) {
      FD_LOG_ERROR(TAG, "Grayscale conversion failed");
      camera->releaseFrame(frame);
      return false;
    }
  } else {
    FD_LOG_ERROR(TAG, "Unsupported pixel format: %d", frame.format);
    camera->releaseFrame(frame);
    return false;
  }
  
  FD_LOG_DEBUG(TAG, "Running detection on %dx%d image", frame.width, frame.height);
  
  uint32_t start_ms = millis();
  std::vector<FaceBox> faces;
  
  int num_faces = detector.detect(gray, frame.width, frame.height, 
                                 frame.stride, faces);
  
  uint32_t elapsed_ms = millis() - start_ms;
  totalDetectionTimeMs += elapsed_ms;
  detectionCount++;
  
  FD_LOG_INFO(TAG, "Detection complete in %u ms: %d face(s) found", elapsed_ms, num_faces);
  
  camera->releaseFrame(frame);
  
  if (ownedGray) {
    delete[] gray;
  }
  
  if (num_faces > 0) {
    std::sort(faces.begin(), faces.end(),
             [](const FaceBox& a, const FaceBox& b) {
               return a.width * a.height > b.width * b.height;
             });
    
    faceBox = faces[0];
    FD_LOG_INFO(TAG, "Largest face: (%d, %d) %dx%d",
               faceBox.x, faceBox.y, faceBox.width, faceBox.height);
    
    return true;
  }
  
  FD_LOG_DEBUG(TAG, "No face detected");
  return false;
}

int FaceDetectESP::detectMultiple(std::vector<FaceBox>& faces) {
  if (!initialized) {
    FD_LOG_ERROR(TAG, "Not initialized");
    return 0;
  }
  
  ImageFrame frame;
  uint32_t ts = 0;
  
  if (!camera->captureFrame(frame, ts)) {
    FD_LOG_WARN(TAG, "Frame capture failed");
    return 0;
  }
  
  lastFrameTimestamp = ts;
  
  uint8_t* gray = nullptr;
  bool ownedGray = false;
  
  if (frame.format == ImageFrame::GRAY8) {
    gray = frame.data;
  } else if (frame.format == ImageFrame::RGB565) {
    gray = convertRGB565toGray(frame.data, frame.width, frame.height);
    ownedGray = true;
    
    if (!gray) {
      camera->releaseFrame(frame);
      return 0;
    }
  } else {
    camera->releaseFrame(frame);
    return 0;
  }
  
  uint32_t start_ms = millis();
  int num_faces = detector.detect(gray, frame.width, frame.height,
                                 frame.stride, faces);
  uint32_t elapsed_ms = millis() - start_ms;
  
  totalDetectionTimeMs += elapsed_ms;
  detectionCount++;
  
  camera->releaseFrame(frame);
  
  if (ownedGray) {
    delete[] gray;
  }
  
  std::sort(faces.begin(), faces.end(),
           [](const FaceBox& a, const FaceBox& b) {
             return a.width * a.height > b.width * b.height;
           });
  
  FD_LOG_INFO(TAG, "Multi-detect: %d faces in %u ms", num_faces, elapsed_ms);
  
  return num_faces;
}

String FaceDetectESP::diagnostics() {
  String json = "{";
  
  json += "\"version\":\"2.0.0\",";
  json += "\"camera\":\"" + String(camera ? camera->getName() : "none") + "\",";
  json += "\"initialized\":" + String(initialized ? "true" : "false") + ",";
  json += "\"detections\":" + String(detectionCount) + ",";
  
  if (detectionCount > 0) {
    uint32_t avg_ms = totalDetectionTimeMs / detectionCount;
    json += "\"avg_detection_ms\":" + String(avg_ms) + ",";
  }
  
  json += "\"width\":" + String(config.width) + ",";
  json += "\"height\":" + String(config.height) + ",";
  json += "\"scale_factor\":" + String(config.scaleFactor) + ",";
  json += "\"cascade\":\"" + String(config.useLBPCascade ? "LBP" : "Haar") + "\"";
  
  json += "}";
  return json;
}

void FaceDetectESP::end() {
  FD_LOG_INFO(TAG, "Shutting down...");
  
  if (detector.cascadeData) {
    detector.end();
  }
  
  if (camera) {
    camera->end();
  }
  
  initialized = false;
  FD_LOG_INFO(TAG, "Shutdown complete");
}

uint8_t* FaceDetectESP::convertRGB565toGray(const uint8_t* rgb, int width, int height) {
  uint8_t* gray = new uint8_t[width * height];
  if (!gray) return nullptr;
  
  const uint16_t* rgb565 = (const uint16_t*)rgb;
  
  for (int i = 0; i < width * height; i++) {
    uint16_t pixel = rgb565[i];
    
    uint8_t r = (uint8_t)((pixel >> 11) & 0x1F) << 3;
    uint8_t g = (uint8_t)((pixel >> 5) & 0x3F) << 2;
    uint8_t b = (uint8_t)(pixel & 0x1F) << 3;
    
    gray[i] = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b);
  }
  
  FD_LOG_DEBUG(TAG, "Converted %dx%d RGB565 to grayscale", width, height);
  return gray;
}

bool FaceDetectESP::initializeCascade() {
  return true;
}
