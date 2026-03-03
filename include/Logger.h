#pragma once

#include <cstddef>
#include <cstdint>

/**
 * @brief Unified logger interface for FaceDetectESP
 * 
 * Provides consistent logging across all components.
 * Logs are emitted via Serial or custom output handler.
 */
class Logger {
public:
  enum Level {
    LEVEL_DEBUG = 0,
    LEVEL_INFO = 1,
    LEVEL_WARN = 2,
    LEVEL_ERROR = 3
  };

  static void setLogLevel(Level level);
  static void log(Level level, const char* tag, const char* fmt, ...);
  
  static void debug(const char* tag, const char* fmt, ...);
  static void info(const char* tag, const char* fmt, ...);
  static void warn(const char* tag, const char* fmt, ...);
  static void error(const char* tag, const char* fmt, ...);
};

#define FD_LOG_DEBUG(tag, fmt, ...) Logger::debug(tag, fmt, ##__VA_ARGS__)
#define FD_LOG_INFO(tag, fmt, ...) Logger::info(tag, fmt, ##__VA_ARGS__)
#define FD_LOG_WARN(tag, fmt, ...) Logger::warn(tag, fmt, ##__VA_ARGS__)
#define FD_LOG_ERROR(tag, fmt, ...) Logger::error(tag, fmt, ##__VA_ARGS__)
