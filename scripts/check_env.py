#!/usr/bin/env python3
import os
import shutil
import subprocess
import sys

def find_esp_camera():
    search_paths = []
    if "IDF_PATH" in os.environ:
        search_paths.append(os.environ["IDF_PATH"])
    
    if os.name == 'nt':
        search_paths.extend([
            os.path.expanduser("~\\AppData\\Local\\Arduino15\\packages\\esp32"),
            os.path.expanduser("~\\.espressif"),
            "C:\\Espressif"
        ])
    else:
        search_paths.extend([
            os.path.expanduser("~/.arduino15/packages/esp32"),
            os.path.expanduser("~/Library/Arduino15/packages/esp32"),
            "/opt",
            "/usr",
            os.path.expanduser("~/.espressif")
        ])

    for base in search_paths:
        if not os.path.isdir(base):
            continue
        for root, _, files in os.walk(base):
            if "esp_camera.h" in files:
                return True
    return False

def main():
    print("=== FaceDetectESP Environment Check ===\n")

    arduino_available = False
    if shutil.which("arduino-cli"):
        try:
            version = subprocess.check_output(["arduino-cli", "version"], text=True).split('\n')[0]
            print(f"✓ Arduino CLI found: {version}")
            arduino_available = True
        except Exception:
            print("✓ Arduino CLI found")
            arduino_available = True
    else:
        print("✗ Arduino CLI not found")

    print()

    idf_available = False
    if shutil.which("idf.py"):
        try:
            version = subprocess.check_output(["idf.py", "--version"], text=True).split('\n')[0]
            print(f"✓ ESP-IDF found: {version}")
            idf_available = True
        except Exception:
            print("✓ ESP-IDF found")
            idf_available = True
    else:
        print("✗ ESP-IDF not found")

    print("\nChecking for camera headers...")
    
    if find_esp_camera():
        print("✓ esp_camera.h found (ESP-IDF camera driver available)")
    else:
        print("✗ esp_camera.h not found (Checked IDF_PATH and common install dirs)")

    print()

    if arduino_available or idf_available:
        print("✓ At least one build system available")
        sys.exit(0)
    else:
        print("✗ No suitable build system found")
        sys.exit(1)

if __name__ == "__main__":
    main()
