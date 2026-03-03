#include "../include/DetectorCore.h"

static const CascadeData g_lbp_cascade = {
  .width = 24,
  .height = 24,
  .numStages = 5,
  .globalThreshold = 50.0f,
  .stages = {
    {
      .numFeatures = 2,
      .threshold = 5.0f,
      .features = {
        {.x = 0, .y = 0, .w = 12, .h = 12, .featureType = 1, .weight = 1.0f, .featureValue = 128},
        {.x = 12, .y = 0, .w = 12, .h = 12, .featureType = 1, .weight = 1.0f, .featureValue = 64}
      }
    },
    {
      .numFeatures = 2,
      .threshold = 4.5f,
      .features = {
        {.x = 0, .y = 12, .w = 12, .h = 12, .featureType = 1, .weight = 0.8f, .featureValue = 96},
        {.x = 12, .y = 12, .w = 12, .h = 12, .featureType = 1, .weight = 1.2f, .featureValue = 200}
      }
    },
    {
      .numFeatures = 2,
      .threshold = 6.0f,
      .features = {
        {.x = 4, .y = 4, .w = 8, .h = 8, .featureType = 1, .weight = 1.0f, .featureValue = 100},
        {.x = 12, .y = 4, .w = 8, .h = 8, .featureType = 1, .weight = 1.0f, .featureValue = 150}
      }
    },
    {
      .numFeatures = 2,
      .threshold = 5.5f,
      .features = {
        {.x = 4, .y = 12, .w = 8, .h = 8, .featureType = 1, .weight = 0.9f, .featureValue = 120},
        {.x = 12, .y = 12, .w = 8, .h = 8, .featureType = 1, .weight = 1.1f, .featureValue = 140}
      }
    },
    {
      .numFeatures = 2,
      .threshold = 7.0f,
      .features = {
        {.x = 6, .y = 6, .w = 6, .h = 6, .featureType = 1, .weight = 1.0f, .featureValue = 110},
        {.x = 12, .y = 6, .w = 6, .h = 6, .featureType = 1, .weight = 1.0f, .featureValue = 130}
      }
    }
  }
};

const CascadeData* getFaceCascade() {
  return &g_lbp_cascade;
}
