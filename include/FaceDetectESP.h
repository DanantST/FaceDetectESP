#pragma once

#include "CameraAdapter.h"
#include "DetectorCore.h"
#include <Arduino.h>

/**
 * @file FaceDetectESP.h
 * @brief Main FaceDetectESP library interface
 * @author FaceDetectESP Project
 * @date 2026-03-03
 * 
 * Hardware-agnostic face detection for ESP32 platforms.
 * Supports multiple camera sources via CameraAdapter abstraction.
 * 
 * @example
 * @code
 * IDFCameraAdapter camera;
 * FaceDetectESP detector(&camera);
 * 
 * detector.begin();
 * 
 * FaceBox box;
 * if (detector.detect(box)) {
 *   Serial.printf("Face at (%d, %d) %dx%d\n", box.x, box.y, box.width, box.height);
 * }
 * @endcode
 */

struct FaceBox {
  int32_t x;          ///< Top-left X coordinate
  int32_t y;          ///< Top-left Y coordinate
  int32_t width;      ///< Bounding box width
  int32_t height;     ///< Bounding box height
};

/**
 * @class FaceDetectESP
 * @brief Main face detection interface
 */
class FaceDetectESP {
public:
  /**
   * @struct Config
   * @brief Configuration parameters for detector
   */
  struct Config {
    int width = 320;              ///< Input image width
    int height = 240;             ///< Input image height
    float scaleFactor = 1.1f;     ///< Cascade scale factor
    int minNeighbors = 3;         ///< Min neighbors for validation
    int minFaceSize = 60;         ///< Minimum face dimension
    bool useLBPCascade = true;    ///< Use LBP (true) or Haar (false)
    bool loadCascadeFromSPIFFS = false;  ///< Load cascade from SPIFFS
  };

  /**
   * @brief Constructor
   * @param camAdapter pointer to initialized CameraAdapter
   * @param cfg configuration struct
   */
  explicit FaceDetectESP(CameraAdapter* camAdapter, const Config& cfg = Config());

  /**
   * @brief Destructor
   */
  ~FaceDetectESP();

  /**
   * @brief Initialize detector
   * @return true if successful
   */
  bool begin();

  /**
   * @brief Detect faces in next camera frame
   * @param[out] faceBox largest detected face
   * @param[out] frameTimestamp optional timestamp
   * @return true if face detected
   * 
   * Synchronous operation. Returns first frame timeout.
   */
  bool detect(FaceBox& faceBox, uint32_t* frameTimestamp = nullptr);

  /**
   * @brief Detect multiple faces
   * @param[out] faces vector of detected faces
   * @return number of faces detected
   */
  int detectMultiple(std::vector<FaceBox>& faces);

  /**
   * @brief Get diagnostic information
   * @return JSON string with stats
   */
  String diagnostics();

  /**
   * @brief Graceful shutdown
   */
  void end();

private:
  CameraAdapter* camera;
  DetectorCore detector;
  Config config;
  bool initialized = false;
  uint32_t lastFrameTimestamp = 0;
  uint32_t detectionCount = 0;
  uint32_t totalDetectionTimeMs = 0;
  
  bool initializeCascade();
  uint8_t* convertRGB565toGray(const uint8_t* rgb, int width, int height);
};
