#include <FaceDetectESP.h>
#include <IDFCameraAdapter.h>

IDFCameraAdapter camera;

FaceDetectESP::Config config;
FaceDetectESP detector(&camera, config);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\n=== FaceDetectESP Arduino Demo ===");
  Serial.println("Version: 2.0.0");
  Serial.println();
  
  config.width = 320;
  config.height = 240;
  config.minFaceSize = 60;
  config.scaleFactor = 1.1f;
  config.minNeighbors = 3;
  config.useLBPCascade = true;
  
  if (!detector.begin()) {
    Serial.println("ERROR: Failed to initialize detector!");
    while (1) {
      delay(1000);
    }
  }
  
  Serial.println("Detector initialized successfully");
  Serial.println("Starting detection loop...\n");
}

void loop() {
  FaceBox box;
  
  if (detector.detect(box)) {
    Serial.print("FACE DETECTED: x=");
    Serial.print(box.x);
    Serial.print(", y=");
    Serial.print(box.y);
    Serial.print(", width=");
    Serial.print(box.width);
    Serial.print(", height=");
    Serial.println(box.height);
  } else {
    Serial.println("No face detected in this frame");
  }
  
  delay(500);
  
  static uint32_t lastDiag = 0;
  if (millis() - lastDiag > 10000) {
    Serial.print("Diagnostics: ");
    Serial.println(detector.diagnostics());
    lastDiag = millis();
  }
}
