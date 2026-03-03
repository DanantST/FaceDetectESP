#include "../include/Logger.h"
#include <Arduino.h>
#include <cstdio>
#include <cstdarg>

static Logger::Level g_logLevel = Logger::LEVEL_INFO;
static char g_logBuffer[256];

void Logger::setLogLevel(Level level) {
  g_logLevel = level;
}

void Logger::log(Level level, const char* tag, const char* fmt, ...) {
  if (level < g_logLevel) {
    return;
  }
  
  const char* levelStr = "???";
  switch (level) {
    case LEVEL_DEBUG: levelStr = "DEBUG"; break;
    case LEVEL_INFO: levelStr = "INFO"; break;
    case LEVEL_WARN: levelStr = "WARN"; break;
    case LEVEL_ERROR: levelStr = "ERROR"; break;
  }
  
  va_list args;
  va_start(args, fmt);
  vsnprintf(g_logBuffer, sizeof(g_logBuffer), fmt, args);
  va_end(args);
  
  Serial.printf("[%s] %s: %s\n", levelStr, tag, g_logBuffer);
}

void Logger::debug(const char* tag, const char* fmt, ...) {
  if (LEVEL_DEBUG < g_logLevel) return;
  
  va_list args;
  va_start(args, fmt);
  vsnprintf(g_logBuffer, sizeof(g_logBuffer), fmt, args);
  va_end(args);
  
  Serial.printf("[DEBUG] %s: %s\n", tag, g_logBuffer);
}

void Logger::info(const char* tag, const char* fmt, ...) {
  if (LEVEL_INFO < g_logLevel) return;
  
  va_list args;
  va_start(args, fmt);
  vsnprintf(g_logBuffer, sizeof(g_logBuffer), fmt, args);
  va_end(args);
  
  Serial.printf("[INFO] %s: %s\n", tag, g_logBuffer);
}

void Logger::warn(const char* tag, const char* fmt, ...) {
  if (LEVEL_WARN < g_logLevel) return;
  
  va_list args;
  va_start(args, fmt);
  vsnprintf(g_logBuffer, sizeof(g_logBuffer), fmt, args);
  va_end(args);
  
  Serial.printf("[WARN] %s: %s\n", tag, g_logBuffer);
}

void Logger::error(const char* tag, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(g_logBuffer, sizeof(g_logBuffer), fmt, args);
  va_end(args);
  
  Serial.printf("[ERROR] %s: %s\n", tag, g_logBuffer);
}
