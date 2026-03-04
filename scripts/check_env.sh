#!/bin/bash

echo "=== FaceDetectESP Environment Check ==="
echo

if command -v arduino-cli &> /dev/null; then
    ARDUINO_VERSION=$(arduino-cli version 2>/dev/null | head -1)
    echo "✓ Arduino CLI found: $ARDUINO_VERSION"
    ARDUINO_AVAILABLE=1
else
    echo "✗ Arduino CLI not found"
    ARDUINO_AVAILABLE=0
fi

echo

if command -v idf.py &> /dev/null; then
    IDF_VERSION=$(idf.py --version 2>/dev/null | head -1)
    echo "✓ ESP-IDF found: $IDF_VERSION"
    IDF_AVAILABLE=1
else
    echo "✗ ESP-IDF not found"
    IDF_AVAILABLE=0
fi

echo

echo "Checking for camera headers..."

if find /opt /usr -name "esp_camera.h" 2>/dev/null | grep -q .; then
    echo "✓ esp_camera.h found (ESP-IDF camera driver available)"
else
    echo "✗ esp_camera.h not found"
fi

echo

if [ $ARDUINO_AVAILABLE -eq 1 ] || [ $IDF_AVAILABLE -eq 1 ]; then
    echo "✓ At least one build system available"
    exit 0
else
    echo "✗ No suitable build system found"
    exit 1
fi
