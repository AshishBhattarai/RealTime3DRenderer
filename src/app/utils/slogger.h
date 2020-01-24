#pragma once

#define CSLOG(...)                                                             \
  SLogger::getInstance().printToConsole(__FILE__, __LINE__, __VA_ARGS__)
#define FSLOG(...)                                                             \
  SLogger::getInstance().printToFile(__FILE__, __LINE__, __VA_ARGS__)
#define SLOG(...)                                                              \
  SLogger::getInstance().printToFileConsole(__FILE__, __LINE__, __VA_ARGS__)
#ifdef NDEBUG
#define DEBUG_SLOG(...)
#define DEBUG_FSLOG(...)
#define DEBUG_CSLOG(...)
#else
#define DEBUG_SLOG(...) SLOG(__VA_ARGS__)
#define DEBUG_FSLOG(...) FSLOG(__VA_ARGS__)
#define DEBUG_CSLOG(...) CSLOG(__VA_ARGS__)
#endif

#include "utils.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Simple Logger

namespace app {

/**
 * A singleton class to print errors/warnings to console and file uses varadic
 * templates
 */
class SLogger {
private:
  FILE *file;
  std::stringstream ss;
  static constexpr const char *logFile = "logs.txt";

  SLogger() : file(fopen(logFile, "a")) {
    if (!file)
      printToConsole(__FILE__, __LINE__, "Failed to open log file", logFile);
    else
      fprintf(file, "\n[LOGGER_INIT]\n\n");
  }

  ~SLogger() {
    if (file)
      fclose(file);
  }

  SLogger(const SLogger &) = delete;
  SLogger &operator=(const SLogger &) = delete;

  // print the message on give output strema
  template <typename... Args>
  void print(FILE *out, std::string fname, size_t line, Args &&... args) {
    // file name
    fname = fname.substr(fname.find_last_of('/') + 1, fname.length());
    ((ss << " " << args), ...) << std::endl;
    fprintf(out, "%s, file: %s in %lu: %s", utils::currentDateTime().c_str(),
            fname.c_str(), line, ss.str().c_str());
    ss.str(std::string());
    ss.clear();
  }

public:
  static SLogger &getInstance() {
    static SLogger logger;
    return logger;
  }

  // print on console
  template <typename... Args>
  void printToConsole(std::string fname, size_t line, Args &&... args) {
    print(stderr, fname, line, args...);
  }

  // print on file
  template <typename... Args>
  void printToFile(std::string fname, size_t line, Args &&... args) {
    print(file, fname, line, args...);
  }

  // print on both file and console
  template <typename... Args>
  void printToFileConsole(std::string fname, size_t line, Args &&... args) {
    print(file, fname, line, args...);
    print(stderr, fname, line, args...);
  }
};

} // namespace app
