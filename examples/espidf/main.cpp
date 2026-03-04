#include <cstdio>
#include <cstring>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "FaceDetectESP.h"
#include "IDFCameraAdapter.h"

static const char* TAG = "FaceDetectESP_Example";

extern "C" void app_main() {
  ESP_LOGI(TAG, "=== FaceDetectESP ESP-IDF Example ===");
  
  IDFCameraAdapter camera;
  
  FaceDetectESP::Config config;
  config.width = 320;
  config.height = 240;
  config.minFaceSize = 60;
  
  FaceDetectESP detector(&camera, config);
  
  if (!detector.begin()) {
    ESP_LOGE(TAG, "Failed to initialize detector");
    vTaskDelete(nullptr);
    return;
  }
  
  ESP_LOGI(TAG, "Detector initialized successfully");
  
  for (int frame = 0; frame < 100; frame++) {
    FaceBox box;
    
    if (detector.detect(box)) {
      ESP_LOGI(TAG, "Face #%d: (%d, %d) %dx%d",
              frame, box.x, box.y, box.width, box.height);
    } else {
      ESP_LOGD(TAG, "Frame %d: No face detected", frame);
    }
    
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  
  ESP_LOGI(TAG, "Final: %s", detector.diagnostics().c_str());
  
  detector.end();
  ESP_LOGI(TAG, "Example complete");
}
