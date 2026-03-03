#pragma once

#include <cstdint>
#include <cstddef>

/**
 * @file CameraAdapter.h
 * @brief Abstract camera interface for FaceDetectESP
 * 
 * Allows FaceDetectESP to work with any camera source through adapters.
 */

struct ImageFrame {
  uint8_t* data;      ///< Pointer to pixel data
  int width;          ///< Frame width in pixels
  int height;         ///< Frame height in pixels
  int stride;         ///< Bytes per row (may include padding)
  
  enum PixelFormat {
    GRAY8 = 0,        ///< 8-bit grayscale
    RGB565 = 1,       ///< 16-bit RGB565
    YUV422 = 2        ///< YUV422 packed
  } format;           ///< Pixel format
};

/**
 * @class CameraAdapter
 * @brief Abstract interface for camera capture
 * 
 * Implementations:
 * - IDFCameraAdapter: ESP-IDF camera_driver for P4 MIPI CSI
 * - ArduinoCameraAdapter: Arduino camera APIs (if available)
 * - FakeCameraAdapter: Test images from memory/SPIFFS
 */
class CameraAdapter {
public:
  virtual ~CameraAdapter() = default;
  
  /**
   * @brief Initialize camera hardware
   * @return true if successful
   */
  virtual bool begin() = 0;
  
  /**
   * @brief Capture one frame synchronously
   * @param[out] frame filled with frame data
   * @param[out] frameTimestampMs timestamp in milliseconds
   * @return true if frame captured
   * 
   * Frame buffer ownership is managed by adapter.
   * Call releaseFrame() when done.
   */
  virtual bool captureFrame(ImageFrame& frame, uint32_t& frameTimestampMs) = 0;
  
  /**
   * @brief Release a captured frame
   * @param frame previously captured via captureFrame()
   */
  virtual void releaseFrame(ImageFrame& frame) = 0;
  
  /**
   * @brief Shutdown camera gracefully
   */
  virtual void end() = 0;
  
  /**
   * @brief Get adapter name (e.g., "IDFCameraAdapter")
   * @return adapter name string
   */
  virtual const char* getName() const = 0;
};
