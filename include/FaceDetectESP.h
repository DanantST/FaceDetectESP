# FaceDetectESP.h

#ifndef FACEDETECTESP_H
#define FACEDETECTESP_H

#include <Arduino.h>

class FaceDetectESP {
public:
    FaceDetectESP();  // Constructor
    ~FaceDetectESP(); // Destructor

    void begin();  // Initialize the library
    void end();    // Clean up resources
    bool detectFace();  // Return true if a face is detected
    void getFaceCoordinates(int &x, int &y, int &width, int &height);  // Get coordinates of the detected face

private:
    // Add any private members or methods needed here
};

#endif // FACEDETECTESP_H